#include "../handlers.h"

void handlers::vmrun(vcpu* vcpu) 
{
	auto& state		= vcpu->get_guest().get_state_save_area();
	auto& control	= vcpu->get_guest().get_control_area();
	
	// we inject an exception here, as we dont want other hypervisors 
	// to be running, and we dont support hypervisor nesting 

	vcpu->inject_exception(EXCEPTION_VECTOR::InvalidOpcode);

	return;
}
