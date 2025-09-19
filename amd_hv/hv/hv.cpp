#include "hv.h"

#include "handlers/handlers.h"

#include "../utilities/utilities.h"

namespace hv 
{
	vcpu** vcpus;
}

bool hv::setup() 
{
	int core_amt = utilities::get_cpu_cores();

	// 8 * amt of cores because its gonna be an array of pointers
	vcpus = (vcpu**)ExAllocatePoolZero(NonPagedPool, 8 * core_amt, 'ENON');

	for (int i = 0; i < core_amt; i++) 
	{
		vcpus[i] = (vcpu*)MmAllocateContiguousMemory(sizeof(vcpu), {.QuadPart = MAXLONG64});
		
		// "Memory that MmAllocateContiguousMemory allocates is uninitialized" 
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-mmallocatecontiguousmemory

		memset(vcpus[i], 0, sizeof(vcpu));
	}

	return true;
}

bool hv::check_loaded()
{
	// we just check current core 
	return send_hv_command(COMMAND_KEY, PING_ID);
}

vcpu* hv::get_vcpu(int idx) 
{
	return vcpus[idx];
}

bool hv::launch() 
{
	// we check this just to be safe

	if (check_loaded())
	{
		LOG_ERROR("hypervisor already loaded \n");
		return true;
	}

	int core_amt = utilities::get_cpu_cores();
	for (int i = 0; i < core_amt; i++) 
	{
		vcpu* vcpu = get_vcpu(i);

		utilities::use_cpu_core(i);

		if (!svm::check_cpu_support())
			return false;

		svm::enable_svm();

		vcpu->setup();

		if (!vcpu->validate_guest()) 
			return false;
		
		if (!start_hv(vcpu))
			return false;

		LOG("core %i virtualization successful! \n", i);
	}

	LOG("cpu fully virtualized! \n");

	return true;
}

bool hv::shutdown() 
{
	if (!check_loaded())
		return false;

	// run shutdown command from kernel on all cores

	int core_amt = utilities::get_cpu_cores();
	for (int i = 0; i < core_amt; i++) 
	{
		utilities::use_cpu_core(i);

		if (!send_hv_command(COMMAND_KEY, SHUTDOWN_ID)) 
		{
			LOG_ERROR("shutdown failed \n");
			return false;
		}
	}

	LOG("cpu fully devirtualized! \n");

	return true;
}

// return true to quit
bool hv::handle_vmexit(vcpu* vcpu) 
{
	auto& control = vcpu->get_guest().get_control_area();

	vcpu->prologue();
	
	// uncomment this to test fsbase being set correctly 
	//if (_readfsbase_u64() == (UINT64)vcpu)
	//	LOG("fsbase is correct! \n");

	switch (control.exit_code) 
	{
	case SVMEXIT::VMRUN:
		handlers::vmrun(vcpu);
		break;
	case SVMEXIT::CPUID:
		
		// we handle commands in cpuid because its available from usermode,
		// and it doesnt cause an exception if the hv isnt loaded, unlike VMRUN instruction

		handlers::cpuid(vcpu); 

		break;
	default:
		break;
	}

	vcpu->epilogue();

	return vcpu->wants_shutdown();
}

void hv::cleanup(vcpu* vcpu) 
{
	auto& control	= vcpu->get_guest().get_control_area();
	auto& state		= vcpu->get_guest().get_state_save_area();

	// we store these on stack so we can access them
	// after their memory has been free-d
	UINT64 next_rip			= control.nrip;
	UINT64 guest_cr3		= state.cr3.AsUInt;
	UINT64 guest_rflags		= state.rflags.AsUInt;
	UINT64 guest_vmcb_phys	= vcpu->get_guest_phys();
	auto regs				= vcpu->get_regs();
	UINT64* guest_rsp		= (UINT64*)state.rsp;

	// restore rax
	regs->rax				= state.rax;

	MmFreeContiguousMemory(vcpu->get_host_stack_base());
	MmFreeContiguousMemory(vcpu);

	// on the last core, free the vcpu array
	if (utilities::get_current_cpu_idx() == utilities::get_cpu_cores() - 1)
		ExFreePoolWithTag(vcpus, 'ENON');

	LOG("hv cleanup! exit rip -> %p \n", next_rip);

	// we pushed rax in assembly before calling this function
	// now, we replace the value of that pushed rax to the target rip
	// so we can just do a ret instruction after cleanup
	// ret instruction does what pop rip would if it existed 
	// so we can leave all registers true to the guest's state
	// otherwise we would have to clobber one register for a jmp -register- instruction

	// -1 to access the pushed value
	guest_rsp[-1] = next_rip;

	__svm_vmload(guest_vmcb_phys);

	__writecr3(guest_cr3);

	__svm_stgi();

	__writeeflags(guest_rflags);

	return;
}