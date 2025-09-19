#include "vcpu.h"

#include "../../utilities/utilities.h"

bool vcpu::setup() 
{
	// AMD64 Manual Volume 2: 15.5 VMRUN Instruction
	// we set up everything that is required by VMRUN instruction here

	auto& control	= guest_vmcb.get_control_area();
	auto& state		= guest_vmcb.get_state_save_area();

	guest_vmcb_phys = MmGetPhysicalAddress(&guest_vmcb).QuadPart;
	host_vmcb_phys	= MmGetPhysicalAddress(&host_vmcb).QuadPart;

	// allocate contiguous memory for better performance 

	host_stack_base	= MmAllocateContiguousMemory(KERNEL_STACK_SIZE, { .QuadPart = MAXLONG64 });

	// need to null it out to initialize it for safety 

	memset(host_stack_base, 0, KERNEL_STACK_SIZE);

	// we need to subtract at least 1 byte off of it because baseaddress + size would point to a page that isnt ours
	// we subtract 0x10 to keep it 0x10 aligned for efficiency 

	host_stack = (char*)host_stack_base + KERNEL_STACK_SIZE - 0x10;

	CONTEXT ctx{};
	RtlCaptureContext(&ctx);

	// AMD64 Manual Volume 2: 15.16 TLB Control
	// we need to set a different ASID (Address Space Identifier) for the guest,
	// so the guest and host virtual address TLB cache doesnt interfere with eachother
	// also if we dont set it vmrun will fail as a safeguard mechanism

	control.guest_asid = 1;

	// initialize cr and dr registers

	state.cr0.AsUInt = __readcr0();
	state.cr2		 = __readcr2();
	state.cr3.AsUInt = __readcr3();
	state.cr4.AsUInt = __readcr4();

	state.dr6.AsUInt = __readdr(6);
	state.dr7.AsUInt = __readdr(7);

	// initialize intercepts

	control.intercept_instructions2.vmrun = 1; // hv wont start without this
	control.intercept_instructions1.cpuid = 1; // set this to show we are hypervised 

	// rip and rsp are set outside of this function
	// we initialize other important registers here

	// rax is set to one to indicate successful vmrun

	state.rax				= 1; 
	state.rflags.AsUInt		= __readeflags();

	// initialize msr-s that vmsave doesnt

	state.efer.value		= __readmsr(IA32_EFER);
	state.g_pat				= __readmsr(IA32_PAT);
	state.debug_ctl.AsUInt	= __readmsr(IA32_DEBUGCTL);

	// initialize segments that vmsave doesnt

	SEGMENT_DESCRIPTOR_REGISTER_64	GDTR, IDTR;

	__sgdt(&GDTR);
	__sidt(&IDTR);

	state.setup_x64_segment(&state.gdtr, GDTR);
	state.setup_x64_segment(&state.idtr, IDTR);

	state.setup_x86_segment(&state.cs, ctx.SegCs);
	state.setup_x86_segment(&state.ds, ctx.SegDs);
	state.setup_x86_segment(&state.es, ctx.SegEs);
	state.setup_x86_segment(&state.ss, ctx.SegSs);


	// AMD64 Manual Volume 2: 15.30.4  VM_HSAVE_PA MSR 
	// we need to provide a valid physical address for the host state save
	// this is different from the vmcb_host we have because thats used to load
	// the host state using vmload, this is used by vmrun automatically upon a VMEXIT

	__writemsr(AMD_MSR::vm_hsave_pa, MmGetPhysicalAddress(&host_save_area).QuadPart);


	// AMD64 Manual Volume 2: 15.5.2  VMSAVE and VMLOAD Instructions
	// vmsave sets up some of the segments/msr-s we missed

	__svm_vmsave(guest_vmcb_phys);
	__svm_vmsave(host_vmcb_phys);

	LOG("guest phys %p host phys %p \n", guest_vmcb_phys, host_vmcb_phys);

	// if we want to make any changes to the host,
	// that could be loaded with vmload we should do it here

	// as an example of customizing the host, lets set fs base to the vcpu pointer 
	// associated with the currently executing cpu core
	// fs base is 0 on windows operating system, and you can set a custom value to it
	// without breaking anything

	// uncomment this to set it, then you can try to access it via _readfsbase_u64 intrinsic
	// inside our vmexit handler!
	
	//host_vmcb.get_state_save_area().fs.base = (UINT64)this;

	return true;
}

// implementation of most checks from AMD64 Manual Volume 2: 15.5 VMRUN Instruction
bool vcpu::validate_guest() 
{
	auto& control	= guest_vmcb.get_control_area();
	auto& state		= guest_vmcb.get_state_save_area();

	auto efer = state.efer;
	if (!efer.svme) 
	{
		LOG_ERROR("EFER.SVME is zero \n");
		return false;
	}

	auto cr0 = state.cr0;
	if (!cr0.CacheDisable && cr0.NotWriteThrough ) 
	{
		LOG_ERROR("CR0.CD is zero and CR0.NW is set \n");
		return false;
	}

	if (cr0.Reserved4) 
	{
		LOG_ERROR("CR0[63:32] are not zero. \n");
		return false;
	}

	auto cr3 = state.cr3;
	if (cr3.Reserved1 || cr3.Reserved2 || cr3.Reserved3) 
	{
		LOG_ERROR("Any MBZ bit of CR3 is set. \n");
		return false;
	}

	auto cr4 = state.cr4;
	if (cr4.Reserved1 || cr4.Reserved2) 
	{
		LOG_ERROR("Any MBZ bit of CR4 is set. \n");
		return false;
	}

	if (cr4.ControlFlowEnforcementEnable && !cr0.WriteProtect)
	{
		LOG_ERROR("CR4.CET=1 when CR0.WP=0 \n");
		return false;
	}

	auto dr6 = state.dr6;
	if (dr6.AsUInt >> 32) 
	{
		LOG_ERROR("DR6[63:32] are not zero. \n");
		return false;
	}

	auto dr7 = state.dr7;
	if (dr7.AsUInt >> 32)
	{
		LOG_ERROR("DR7[63:32] are not zero. \n");
		return false;
	}

	if (efer.reserved1 || efer.reserved2 || efer.reserved3 || efer.reserved4) 
	{
		LOG_ERROR("Any MBZ bit of EFER is set. \n");
		return false;
	}

	// we skip any long mode checks because any amd cpu that supports svm and virtualization is guaranteed to be x64

	if (!control.intercept_instructions2.vmrun)
	{
		LOG_ERROR("The VMRUN intercept bit is clear. \n");
		return false;
	}

	// the following two checks include the check 
	// "The MSR or IOIO intercept tables extend to a physical address that is greater than or equal to the maximum supported physical address."
	// because it checks if the physical address itself is valid for the cpu to use

	auto iopm_base_virt = MmGetVirtualForPhysical({ .QuadPart = (INT64)control.iopm_base_phys });
	if (control.iopm_base_phys && !MmIsAddressValid(iopm_base_virt))
	{
		LOG_ERROR("iopm_base_phys is invalid. \n");
		return false;
	}

	auto msrpm_base_virt = MmGetVirtualForPhysical({ .QuadPart = (INT64)control.msrpm_base_phys });
	if (control.msrpm_base_phys && !MmIsAddressValid(msrpm_base_virt))
	{
		LOG_ERROR("msrpm_base_phys is invalid. \n");
		return false;
	}

	if (!control.guest_asid)
	{
		LOG_ERROR("ASID is equal to zero. \n");
		return false;
	}

	// this check is not in the amd manual but useful to check this just in case

	INT64 hsave_pa = __readmsr(AMD_MSR::vm_hsave_pa);
	auto hsave_va = MmGetVirtualForPhysical({ .QuadPart = hsave_pa });
	if (!hsave_pa || !MmIsAddressValid(hsave_va))
	{
		LOG_ERROR("vm_hsave_pa is invalid. \n");
		return false;
	}

	return true;
}

void vcpu::prologue()
{
	// we backup rax because we are using it for guest vmcb physical address
	// we complete the general registers class to fully represent the guest's registers for 
	// better quality of life and ease of use

	backup_rax = regs->rax;
	regs->rax  = guest_vmcb.get_state_save_area().rax;
	regs->rsp  = guest_vmcb.get_state_save_area().rsp;

	// we dont need to backup or restore rsp because its never overwriting it on POPAQ macro

	// mark everything as cached for better vmrun performance
	// later on some parts of the VMCB may be changed which would require 
	// modifying the VMCB_CLEAN cache so that vmrun loads that data from the VMCB and not from cache

	guest_vmcb.get_control_area().vmcb_clean.reset();

	// The VMRUN instruction reads, but does not change, thevalue of the TLB_CONTROL field.
	// we must make sure to restore this to its idle state to avoid unnecessary tlb flushes
	
	guest_vmcb.get_control_area().tlb_ctl = TLB_CONTROL::dont_flush;

	return;
}

void vcpu::epilogue()
{
	// save any modifications into the guest's state 

	guest_vmcb.get_state_save_area().rax = regs->rax;
	guest_vmcb.get_state_save_area().rsp = regs->rsp;

	// restore our rax

	regs->rax = backup_rax;

	return;
}

VMCB& vcpu::get_guest() 
{
	return guest_vmcb;
}

VMCB& vcpu::get_host() 
{
	return host_vmcb;
}

GENERAL_REGISTERS* vcpu::get_regs()
{
	return regs;
}

UINT64 vcpu::get_guest_phys() 
{
	return guest_vmcb_phys;
}

UINT64 vcpu::get_host_phys()
{
	return host_vmcb_phys;
}

void* vcpu::get_host_stack_base()
{
	return host_stack_base;
}

UINT8& vcpu::wants_shutdown()
{
	return should_shutdown;
}

void vcpu::inject_exception(EXCEPTION_VECTOR exception, INTERRUPTION_TYPE type, int error )
{
	auto& event = guest_vmcb.get_control_area().event_inject;

	// which exceptions can have error codes
	// we use a static array for best performance
	// sources: https://wiki.osdev.org/Exceptions, ia32.h

	static constexpr bool is_error[]{
		false, // #DE   - Divide Error.
		false, // #DB   - Debug.
		false, // #NMI  - Nonmaskable Interrupt.
		false, // #BP   - Breakpoint.
		false, // #OF   - Overflow.
		false, // #BR   - BOUND Range Exceeded.
		false, // #UD   - Invalid Opcode (Undefined Opcode).
		false, // #NM   - Device Not Available (No Math Coprocessor).
		true,  // #DF   - Double Fault.
		false, // #\##  - Coprocessor Segment Overrun (reserved).
		true,  // #TS   - Invalid TSS.
		true,  // #NP   - Segment Not Present.
		true,  // #SS   - Stack Segment Fault.
		true,  // #GP   - General Protection.
		true,  // #PF   - Page Fault.
		false, // #\##	- reserved
		false, // #MF   - Floating-Point Error (Math Fault).
		true,  // #AC   - Alignment Check.
		false, // #MC   - Machine Check.
		false, // #XM   - SIMD Floating-Point Numeric Error.
		false, // #VE   - Virtualization Exception.
		true   // #CP   - Control Protection Exception.
	};

	event.vector = exception;
	event.type	 = exception == EXCEPTION_VECTOR::Nmi ? INTERRUPTION_TYPE::NonMaskableInterrupt : type;
	event.valid  = 1;

	if (is_error[exception])
	{
		event.ev = 1;
		event.error_code = error;
	}

	return;
}