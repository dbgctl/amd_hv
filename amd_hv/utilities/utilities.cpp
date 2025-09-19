#include "utilities.h"

int utilities::get_cpu_cores() {
	return KeQueryActiveProcessorCount(0);
}

ULONG utilities::get_current_cpu_idx() {

	PROCESSOR_NUMBER cpu_num{};

	KeGetCurrentProcessorNumberEx(&cpu_num);

	return KeGetProcessorIndexFromNumber(&cpu_num);
}

void utilities::use_cpu_core(UINT64 idx) {
	return KeSetSystemAffinityThread(1ull << idx);
}