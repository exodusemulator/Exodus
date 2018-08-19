#include "DeviceInternal.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DeviceInternal::DeviceInternal()
:_deviceContext(0)
{}

//----------------------------------------------------------------------------------------
DeviceInternal::~DeviceInternal()
{
	delete _deviceContext;
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int DeviceInternal::GetIDeviceVersion() const
{
	return ThisIDeviceVersion();
}

//----------------------------------------------------------------------------------------
//Device context functions
//----------------------------------------------------------------------------------------
IDeviceContext* DeviceInternal::GetDeviceContext() const
{
	return _deviceContext;
}

//----------------------------------------------------------------------------------------
double DeviceInternal::GetCurrentTimesliceProgress() const
{
	return _deviceContext->GetCurrentTimesliceProgress();
}
