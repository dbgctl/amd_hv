#pragma once

#include "../svm/svm.h"

__declspec(align(0x1000)) struct vcpu
{
private:
	VMCB	guest_vmcb;
	VMCB	host_vmcb;				// you load this via vmload after a vmexit and vmsave-ing the vmexit-ed guest state!
	char	host_save_area[0x1000];	// host stuff gets saved here by the cpu
	void*	host_stack_base;		// ptr to the allocation
	void*	host_stack;				// this is the top of the allocation, just as how the stack will be used

	UINT64	guest_vmcb_phys;		// these are for vmsave, vmload, vmrun instructions
	UINT64	host_vmcb_phys;			// these are for vmsave, vmload, vmrun instructions

	GENERAL_REGISTERS* regs;
	UINT64	backup_rax;
	UINT8	should_shutdown;

public:

	bool setup();

	bool validate_guest();

	void prologue();

	void epilogue();

	VMCB& get_guest();

	VMCB& get_host();

	GENERAL_REGISTERS* get_regs();

	UINT64 get_guest_phys();

	UINT64 get_host_phys();

	void* get_host_stack_base();

	UINT8& wants_shutdown();

	void inject_exception(EXCEPTION_VECTOR exception, INTERRUPTION_TYPE type = INTERRUPTION_TYPE::HardwareException, int error = 0);
};