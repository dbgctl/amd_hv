#include "svm.h"

#include "../../utilities/utilities.h"

bool svm::check_cpu_support() 
{
	char vendor[13];
	__cpuid((int*)vendor, 0);

	// we check if the cpu's vendor name is intel or not
	// we do this to skip having to check if the cpu is vmware vendor

	if (vendor[0] == 'G') // GenuineIntel
	{
		LOG_ERROR("cpu isnt supported \n");
		return false;
	}

	AMD_VM_CR_MSR vmcr{.value = __readmsr(AMD_MSR::vm_cr) };

	// AMD64 Manual Volume 2: 15.30.1  VM_CR MSR 
	// "LOCK—Bit 3. When this bit is set, writes to LOCK and SVMDIS are silently ignored."
	// in most cases, lock bit is set by the bios to 1 to lock the svmdis to 1 when virtualization is disabled
	// to unlock the lock bit in this case, we need to go inside the bios, then find virtualization option and enable it

	if (vmcr.lock == 1 && vmcr.svmdis == 1) 
	{
		LOG_ERROR("virtualization isnt enabled in bios \n");
		return false;
	}

	// "SVMDIS—Bit 4. When this bit is set, writes to EFER treat the SVME bit as MBZ."
	// we set this to be able to write svme bit in efer

	vmcr.svmdis = 0;

	// lock it just in case anything tries to disable it

	vmcr.lock = 1;

	__writemsr(AMD_MSR::vm_cr, vmcr.value);
	
	return true;
}

void svm::enable_svm() 
{
	AMD_EFER_MSR efer{ .value = __readmsr(IA32_EFER) };

	// enable svm inside efer so we can use svm instructions
	// 
	// AMD64 Manual Volume 2: 3.1.7  Extended Feature Enable Register (EFER)
	// "When this bit is zero, the SVM instructions cause #UD exceptions."

	efer.svme = 1;

	__writemsr(IA32_EFER, efer.value);

	return;
}