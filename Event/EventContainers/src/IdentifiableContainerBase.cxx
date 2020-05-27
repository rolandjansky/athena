/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "EventContainers/IdentifiableContainerBase.h"
#include <algorithm>
#include "EventContainers/IDC_WriteHandleBase.h"
#include "CxxUtils/AthUnlikelyMacros.h"
#include "EventContainers/InternalOnline.h"
#include "EventContainers/InternalOffline.h"


using namespace EventContainers;

  IdentifiableContainerBase::IdentifiableContainerBase(EventContainers::IdentifiableCacheBase *cache)
  {
  	m_link = std::make_unique<EventContainers::InternalOnline>(cache);
    m_OnlineMode = true;
  }

  IdentifiableContainerBase::IdentifiableContainerBase(size_t max){
    m_OnlineMode = false;
    m_link = std::make_unique<EventContainers::InternalOffline>(max);
  }

  void  IdentifiableContainerBase::cleanup(deleter_f* deleter){
    if(m_OnlineMode) throw std::runtime_error("Not implemented in online mode");
    static_cast<EventContainers::InternalOffline*>(m_link.get())->cleanUp(deleter);
  }
