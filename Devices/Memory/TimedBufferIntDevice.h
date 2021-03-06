#ifndef __TIMEDBUFFERINTDEVICE_H__
#define __TIMEDBUFFERINTDEVICE_H__
#include "MemoryWrite.h"
#include "TimedBufferInt.h"
#include "TimedBuffers/TimedBuffers.pkg"

class TimedBufferIntDevice :public MemoryWrite, public ITimedBufferIntDevice
{
public:
	// Constructors
	TimedBufferIntDevice(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID);
	virtual bool Construct(IHierarchicalStorageNode& node);

	// Memory size functions
	virtual unsigned int GetMemoryEntrySizeInBytes() const;

	// Initialization functions
	virtual void Initialize();

	// Memory interface functions
	virtual void TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext);
	virtual void TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext);

	// Debug memory access functions
	virtual unsigned int ReadMemoryEntry(unsigned int location) const;
	virtual void WriteMemoryEntry(unsigned int location, unsigned int data);

	// Savestate functions
	virtual void LoadState(IHierarchicalStorageNode& node);
	virtual void SaveState(IHierarchicalStorageNode& node) const;
	virtual void LoadDebuggerState(IHierarchicalStorageNode& node);
	virtual void SaveDebuggerState(IHierarchicalStorageNode& node) const;

	// Memory locking functions
	virtual bool IsMemoryLockingSupported() const;
	virtual void LockMemoryBlock(unsigned int location, unsigned int size, bool state);
	virtual bool IsAddressLocked(unsigned int location) const;

	// Buffer functions
	ITimedBufferInt* GetTimedBuffer();

private:
	TimedBufferInt _bufferShell;
	bool _initialMemoryDataSpecified;
	bool _repeatInitialMemoryData;
	std::vector<unsigned char> _initialMemoryData;
};

#endif
