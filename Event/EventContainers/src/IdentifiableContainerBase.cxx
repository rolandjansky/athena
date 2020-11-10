/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "EventContainers/IdentifiableContainerBase.h"
#include <algorithm>
#include "EventContainers/IDC_WriteHandleBase.h"
#include "CxxUtils/AthUnlikelyMacros.h"
#include "EventContainers/InternalOnline.h"
#include "EventContainers/InternalOffline.h"
#include "EventContainers/InternalOfflineFast.h"
#include "EventContainers/InternalOfflineMap.h"

using namespace EventContainers;

  IdentifiableContainerBase::IdentifiableContainerBase(EventContainers::IdentifiableCacheBase *cache)
  {
    m_link = std::make_unique<EventContainers::InternalOnline>(cache);
    m_OnlineMode = true;
  }

  IdentifiableContainerBase::IdentifiableContainerBase(size_t max)
    : m_OnlineMode (false),
      m_link (std::make_unique<EventContainers::InternalOffline>(max))
  {
  }


  IdentifiableContainerBase::IdentifiableContainerBase(size_t max, Mode mode){
    m_OnlineMode = false;
    if(mode == Mode::OfflineLowMemory) m_link = std::make_unique<EventContainers::InternalOffline>(max);
    else if(mode == Mode::OfflineFast) m_link = std::make_unique<EventContainers::InternalOfflineFast>(max);
    else if(mode == Mode::OfflineMap)  m_link = std::make_unique<EventContainers::InternalOfflineMap>(max);
    else{
      throw std::runtime_error("Invalid Mode specified");
    }
  }

  void  IdentifiableContainerBase::cleanup(deleter_f* deleter){
    if(m_OnlineMode) throw std::runtime_error("Not implemented in online mode");
    m_link->cleanUp(deleter);
  }
