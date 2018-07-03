/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "EventContainers/IDC_Wait.h"
#include "EventContainers/IdentifiableCacheBase.h"
#include <algorithm>

using namespace EventContainers;

void IDC_Wait::AddWait(std::atomic<const void*>* at){
   m_locklist.emplace_back(at);
}

void IDC_Wait::Wait(){

    const void* waitstate = reinterpret_cast<const void*>(IdentifiableCacheBase::INVALIDflag);
    while(!m_locklist.empty()){
       m_locklist.erase(std::remove_if(m_locklist.begin(), m_locklist.end(),
            [waitstate](const std::atomic<const void*>* o) { return o->load(std::memory_order_acquire) != waitstate; }),
              m_locklist.end());
    }
}

void IDC_Wait::WaitAndClear(){
   Wait();
   m_locklist.clear();
}

IDC_Wait::~IDC_Wait(){
   Wait();
}

