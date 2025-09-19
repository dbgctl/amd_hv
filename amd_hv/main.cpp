
#include <ntifs.h>
#include <windef.h>

#include "hv/svm/svm.h"
#include "utilities/utilities.h"
#include "hv/hv.h"

#include <intrin.h>

void UnloadDriver(PDRIVER_OBJECT driver_object)
{
	UNREFERENCED_PARAMETER(driver_object);

	hv::shutdown();

	return;
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path)
{
	UNREFERENCED_PARAMETER(registry_path);

	driver_object->DriverUnload = UnloadDriver;

	if (!hv::setup())
		return STATUS_NOT_SUPPORTED;

	if (!hv::launch())
		return STATUS_FAILED_DRIVER_ENTRY;

	return STATUS_SUCCESS;
}