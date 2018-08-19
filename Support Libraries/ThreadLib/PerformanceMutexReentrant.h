#ifndef __PERFORMANCEMUTEXREENTRANT_H__
#define __PERFORMANCEMUTEXREENTRANT_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "InterlockedTypes.h"

struct PerformanceMutexReentrant
{
public:
	inline PerformanceMutexReentrant()
	:_threadIDMutex(0), _lockCount(0)
	{}

	inline void Lock()
	{
		//Generate an explicit read/write barrier. This is not a memory barrier! This is a
		//compiler intrinsic to prevent instruction re-ordering. In this case, this
		//barrier will ensure that even if this function is inlined by the compiler, any
		//preceding instructions that work with shared memory will be performed before
		//this code is executed, ensuring that instructions are not re-ordered around the
		//mutex.
		_ReadWriteBarrier();

		//Obtain the current thread ID
		InterlockedVar32 currentThreadID = (InterlockedVar32)GetCurrentThreadId();

		//Obtain a lock for this thread
		while(PerformanceInterlockedCompareExchangeAcquire(&_threadIDMutex, LockAcquireOrReleaseInProgress, 0) != 0)
		{
			//If this mutex is already locked by the current thread, increment the lock
			//count, and return.
			if(PerformanceInterlockedCompareExchangeAcquire(&_threadIDMutex, LockAcquireOrReleaseInProgress, currentThreadID) == currentThreadID)
			{
				//Increment the lock count for this thread
				++_lockCount;

				//Generate an explicit write barrier. This is not a memory barrier! This
				//is a compiler intrinsic to prevent instruction re-ordering. This
				//statement will force changes to the lockCount variable to be made in
				//code before we call InterlockedCompareExchangeRelease below. Note that
				//this does not guarantee our changes will be visible to other processors
				//before the change to threadIDMutex, for that we need a memory barrier,
				//which is generated for us by InterlockedCompareExchangeRelease.
				_WriteBarrier();

				//Flag that a new lock acquire is no longer in progress. Note that while
				//we have an exclusive lock on threadIDMutex here, we still need the
				//memory barrier generated by this method here to ensure that our changes
				//to memory are fully visible to other processors.
				PerformanceInterlockedCompareExchangeRelease(&_threadIDMutex, currentThreadID, LockAcquireOrReleaseInProgress);

				//This final write barrier ensures that the compiler doesn't re-order our
				//acquire of the mutex after any following instructions.
				_WriteBarrier();

				return;
			}
		}

		//Set the lock count to 1 now that we've obtained a new lock for this thread
		_lockCount = 1;

		//Generate an explicit write barrier. This is not a memory barrier! This is a
		//compiler intrinsic to prevent instruction re-ordering. This statement will force
		//changes to the lockCount variable to be made in code before we call
		//InterlockedCompareExchangeRelease below. Note that this does not guarantee our
		//changes will be visible to other processors before the change to threadIDMutex,
		//for that we need a memory barrier, which is generated for us by
		//InterlockedCompareExchangeRelease.
		_WriteBarrier();

		//Flag that the new lock has now been fully obtained by setting the thread mutex
		//lock to the current thread ID. Note that while we have an exclusive lock on
		//threadIDMutex here, we still need the memory barrier generated by this method
		//here to ensure that our changes to memory are fully visible to other processors.
		PerformanceInterlockedCompareExchangeRelease(&_threadIDMutex, currentThreadID, LockAcquireOrReleaseInProgress);

		//This final write barrier ensures that the compiler doesn't re-order our acquire
		//of the mutex after any following instructions.
		_WriteBarrier();
	}

	inline void Unlock()
	{
		//Generate an explicit read/write barrier. This is not a memory barrier! This is a
		//compiler intrinsic to prevent instruction re-ordering. In this case, this
		//barrier will ensure that even if this function is inlined by the compiler, any
		//preceding instructions that work with shared memory will be performed before
		//this code is executed, ensuring that instructions are not re-ordered around the
		//mutex.
		_ReadWriteBarrier();

		//Obtain the current thread ID
		InterlockedVar32 currentThreadID = (InterlockedVar32)GetCurrentThreadId();

		//Wait until we manage to grab an exclusive lock on the mutex for this thread.
		//Note that if the current thread doesn't have a lock on the mutex, this will loop
		//forever.
		while(PerformanceInterlockedCompareExchangeAcquire(&_threadIDMutex, LockAcquireOrReleaseInProgress, currentThreadID) != currentThreadID)
		{}

		//Decrement the lock count for this thread, and flag that we should now release
		//the thread ID lock if the lock count has reached 0. Note that documentation
		//confirms that "0" is not a valid thread ID number in Windows, so we can use it
		//to indicate an unlocked mutex.
		InterlockedVar32 newThreadIDLockValue = (--_lockCount == 0)? 0: currentThreadID;

		//Generate an explicit write barrier. This is not a memory barrier! This is a
		//compiler intrinsic to prevent instruction re-ordering. This statement will force
		//changes to the lockCount variable to be made in code before we call
		//InterlockedCompareExchangeRelease below. Note that this does not guarantee our
		//changes will be visible to other processors before the change to threadIDMutex,
		//for that we need a memory barrier, which is generated for us by
		//InterlockedCompareExchangeRelease.
		_WriteBarrier();

		//If the lock count has now reached 0, release the lock for this thread, otherwise
		//flag that the release operation is complete. Note that while we have an
		//exclusive lock on threadIDMutex here, we still need the memory barrier generated
		//by this method here to ensure that our changes to memory are fully visible to
		//other processors.
		PerformanceInterlockedCompareExchangeRelease(&_threadIDMutex, newThreadIDLockValue, LockAcquireOrReleaseInProgress);

		//This final write barrier ensures that the compiler doesn't re-order our release
		//of the mutex after any following instructions.
		_WriteBarrier();
	}

private:
	static const InterlockedVar32 LockAcquireOrReleaseInProgress = (InterlockedVar32)-1;

private:
	//Note that we need to use the align directive here to guarantee that our interlocked
	//types will be correctly aligned to a memory boundary. All the interlocked memory
	//functions require this alignment, otherwise the result is undefined.
	__declspec(align(32)) volatile InterlockedVar32 _threadIDMutex;
	unsigned int _lockCount;
};

#endif
