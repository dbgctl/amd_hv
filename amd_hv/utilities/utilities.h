#pragma once

#include <ntifs.h>
#include <windef.h>

#ifdef _DEBUG
#define LOG(str, ...) utilities::debug_log(str, __VA_ARGS__) 
#else
#define LOG(str, ...)
#endif

#define LOG_ERROR(str) utilities::debug_log("error: %s \n", str)


namespace utilities 
{
	// gets the amount of runnable cpu cores in system
	int get_cpu_cores();

	// gets the index of the core that is executing this thread currently 
	ULONG get_current_cpu_idx();

	// only uses a specific core to run this thread
	void use_cpu_core(UINT64 idx);

	template< typename... VA >
	void debug_log(const char* msg, VA&&... args) 
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, msg, args...);
		return;
	}
}