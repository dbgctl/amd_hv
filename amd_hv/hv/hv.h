#pragma once

#include "vcpu/vcpu.h"

#define SHUTDOWN_ID 0x124
#define PING_ID 0x123
#define COMMAND_KEY 0x123456789

namespace hv 
{
	bool setup();

	bool check_loaded();

	extern vcpu** vcpus;
	vcpu* get_vcpu(int idx);

	bool launch();

	bool shutdown();

	bool handle_vmexit(vcpu* vcpu);

	// cleanup the allocations
	void cleanup(vcpu* vcpu);
}