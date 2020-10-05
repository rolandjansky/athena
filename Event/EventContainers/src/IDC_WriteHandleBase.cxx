/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "EventContainers/IDC_WriteHandleBase.h"
#include "EventContainers/IdentifiableCacheBase.h"


using namespace EventContainers;



IDC_WriteHandleBase::~IDC_WriteHandleBase() { ReleaseLock(); }

//If object has not been filled this will change state to aborted
//If object has been filled this should do nothing
void IDC_WriteHandleBase::ReleaseLock(){
   if(m_atomic==nullptr) return;
//Convenience declarations
   typedef std::scoped_lock<decltype(m_mut->mutex)> lockguard;
   const void* waitstate = reinterpret_cast<const void*>(IdentifiableCacheBase::INVALIDflag);
   const void* ABORTstate = reinterpret_cast<const void*>(IdentifiableCacheBase::ABORTEDflag);

//Running code
   assert(m_atomic->load() != ABORTstate);
   lockguard lk(m_mut->mutex);
   m_atomic->compare_exchange_strong(waitstate, ABORTstate);
   m_mut->condition.notify_all();
   m_atomic = nullptr;
}

//Can't imagine why this would be used but this may be necessary
void IDC_WriteHandleBase::DropLock() noexcept{
   m_atomic = nullptr;
}

