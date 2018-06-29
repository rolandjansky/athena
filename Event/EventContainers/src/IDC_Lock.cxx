/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "EventContainers/IDC_Lock.h"
#include "EventContainers/IdentifiableCacheBase.h"


using namespace EventContainers;
void IDC_Lock::ReleaseLock(){//IF object has not been filled this will restore unfilled state
                      //If object has been filled this should do nothing
   if(m_atomic==nullptr) return;
   using namespace EventContainers; 
   const void* waitstate = reinterpret_cast<const void*>(IdentifiableCacheBase::INVALIDflag);
   const void* ABORTstate = reinterpret_cast<const void*>(IdentifiableCacheBase::ABORTEDflag);
   std::unique_lock<decltype(m_mut->mutex) > lk(m_mut->mutex);
   m_atomic->compare_exchange_strong(waitstate, ABORTstate);
   m_mut->condition.notify_all();
}

void IDC_Lock::DropLock(){
   m_atomic = nullptr;
}

