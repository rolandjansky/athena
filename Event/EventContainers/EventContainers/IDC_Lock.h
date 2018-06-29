/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef IDC_LOCK_H
#define IDC_LOCK_H
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <utility>

namespace EventContainers {

struct mutexPair{
   std::condition_variable condition;
   std::mutex mutex;
   mutexPair() : condition(), mutex() {}
};


class IDC_Lock{

   std::atomic<const void*>*  m_atomic;
   mutexPair *m_mut;   

public:
   IDC_Lock() : m_atomic(nullptr) { }
   void LockOn(std::atomic<const void*>* in, mutexPair *pair){
      m_atomic = in;
      m_mut    = pair;
   }
   void DropLock();
   void ReleaseLock();

   ~IDC_Lock() { ReleaseLock(); }
};

}
#endif

