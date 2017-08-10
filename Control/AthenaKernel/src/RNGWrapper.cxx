/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandomEngine.h"

ATHRNG::RNGWrapper::RNGWrapper(factoryFunc& genFact, size_t nSlots)
  : m_evNum(0), m_runNum(0)
{
  // Construct the random engines; one per event slot.
  m_engines.reserve(nSlots);
  for(size_t t = 0; t < nSlots; t++){
    m_engines.emplace_back(genFact());
  }
}

ATHRNG::RNGWrapper::~RNGWrapper(){}

bool ATHRNG::RNGWrapper::setSeed(const std::string& algName, const EventContext& ctx)
{
  if(!ctx.valid()){
    return false;
  }
  if ((m_evNum!=ctx.eventID().event_number())||(m_runNum!=ctx.eventID().run_number())){
    m_evNum=ctx.eventID().event_number();
    m_runNum=ctx.eventID().run_number();
    auto algHash=std::hash<std::string>{}(algName);
    auto evHash=std::hash<decltype(m_evNum)>{}(m_evNum);
    auto runHash=std::hash<decltype(m_runNum)>{}(m_evNum);
    auto hsh=evHash^(runHash+(evHash<<6)+(evHash>>2));
    hsh=hsh^(algHash+(hsh<<6)+(hsh>>2));
    m_engines[ctx.slot()]->setSeed(hsh,0);
    return true;
  }
  return false;
}

bool ATHRNG::RNGWrapper::setSeed(const std::string& algName, size_t slot,
                                 uint64_t ev, uint64_t run)
{
  if ((m_evNum != ev) || (m_runNum != run)){
    m_evNum = ev;
    m_runNum = run;
    auto algHash = std::hash<std::string>{}(algName);
    auto evHash = std::hash<decltype(m_evNum)>{}(m_evNum);
    auto runHash = std::hash<decltype(m_runNum)>{}(m_evNum);
    auto hsh = evHash ^ (runHash + (evHash << 6) + (evHash >> 2));
    hsh = hsh ^ (algHash + (hsh << 6) + (hsh >> 2));
    m_engines[slot]->setSeed(hsh, 0);
    return true;
  }
  return false;
}
