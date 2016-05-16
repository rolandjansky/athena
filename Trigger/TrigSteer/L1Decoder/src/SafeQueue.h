/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
* Author: Piotr Sarna
*/

#ifndef L1Decoder_SafeQueue_h
#define L1Decoder_SafeQueue_h

#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>

class SafeQueueDeletedException : public std::exception {};

template <class T>
class SafeQueue
{
public:
	SafeQueue() : mDestructorFired(false) { }

	~SafeQueue() {
		std::lock_guard<std::mutex> lock(mMutex);
		mDestructorFired = true;
		mConditionVariable.notify_all();
	}

	bool empty() const {
		if (mDestructorFired) {
			throw SafeQueueDeletedException();
		}

		std::lock_guard<std::mutex> lock(mMutex);

		return mQueue.empty();
	}

	void enqueue(T t) {
		if (mDestructorFired) {
			throw SafeQueueDeletedException();
		}

		std::lock_guard<std::mutex> lock(mMutex);
		mQueue.push(t);
		mConditionVariable.notify_one();
	}

	T dequeue() {
		if (mDestructorFired) {
			throw SafeQueueDeletedException();
		}

		std::unique_lock<std::mutex> lock(mMutex);

		while (mQueue.empty()) {
			mConditionVariable.wait(lock);

			if (mDestructorFired) {
				throw SafeQueueDeletedException();
			}
		}

		T val = mQueue.front();
		mQueue.pop();

		return val;
	}
private:
	bool mDestructorFired;
	std::queue<T> mQueue;
	mutable std::mutex mMutex;
	std::condition_variable mConditionVariable;
};
#endif //L1Decoder_SafeQueue_h