#include "../handlers.h"

#include "../../../utilities/utilities.h"

void handlers::cpuid(vcpu* vcpu)
{
	auto& state		= vcpu->get_guest().get_state_save_area();
	auto& control	= vcpu->get_guest().get_control_area();
	auto regs		= vcpu->get_regs();

	// if this intercept isnt from us sending a command to the hypervisor
	// we need to mimic cpuid instruction as close to bare metal as possible 

	if (regs->rcx != COMMAND_KEY)
	{
		int cpuid_regs[4];

		// execute cpuid instruction with the guest's parameters and store the result

		__cpuidex(cpuid_regs, regs->rax, regs->rcx);

		// to be as accurate to hardware as possible, we only write 
		// to the lower 4 bytes of rax register
		// cpuid does this on bare metal so we must replicate it

		constexpr UINT64 upper_4bytes = MAXUINT64 - MAXUINT32;

		// we preserve the upper 4 bytes, then add the resulting lower 4 bytes 

		regs->rax = (regs->rax & upper_4bytes) + cpuid_regs[0];
		regs->rbx = (regs->rbx & upper_4bytes) + cpuid_regs[1];
		regs->rcx = (regs->rcx & upper_4bytes) + cpuid_regs[2];
		regs->rdx = (regs->rdx & upper_4bytes) + cpuid_regs[3];

		goto ret;
	}

	// set rax to 1 to indicate handled
	
	regs->rax = 1; 

	switch (regs->rdx) 
	{
	case PING_ID: 

		// ping should just return if its handled or not, and we set handled a few lines above

		break;
	case SHUTDOWN_ID:

		// only shutdown if its a command sent from kernel because we need to exit into a kernel rip and context

		if (!state.cpl)
			vcpu->wants_shutdown() = 1;
		break;
	default:

		// set rax to 0 to indicate unhandled

		regs->rax = 0;
	}

	ret:
	state.rip = control.nrip;

	return;
}
