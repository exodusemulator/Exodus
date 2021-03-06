#ifndef __ICLOCKSOURCE_H__
#define __ICLOCKSOURCE_H__
class IDeviceContext;

class IClockSource
{
public:
	// Enumerations
	enum class ClockType;

public:
	// Constructors
	inline virtual ~IClockSource() = 0;

	// Interface version functions
	static inline unsigned int ThisIClockSourceVersion() { return 1; }
	virtual unsigned int GetIClockSourceVersion() const = 0;

	// Clock type functions
	virtual ClockType GetClockType() const = 0;

	//##TODO## Add support for clock phase, so that the progress of the current clock
	// pulse can be retrieved and set.

	// Clock frequency functions
	virtual bool SetClockFrequency(double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext) = 0;
	virtual bool SetClockDivider(double divider, IDeviceContext* caller, double accessTime, unsigned int accessContext) = 0;
	virtual bool SetClockMultiplier(double multiplier, IDeviceContext* caller, double accessTime, unsigned int accessContext) = 0;
	virtual bool TransparentSetClockFrequency(double clockRate) = 0;
	virtual bool TransparentSetClockDivider(double divider) = 0;
	virtual bool TransparentSetClockMultiplier(double multiplier) = 0;
};
IClockSource::~IClockSource() { }

#include "IClockSource.inl"
#endif
