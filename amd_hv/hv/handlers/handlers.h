#pragma once

#include "../hv.h"

namespace handlers 
{

	void vmrun(vcpu* vcpu);

	void cpuid(vcpu* vcpu);

}
