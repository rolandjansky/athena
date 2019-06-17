/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CostData.h"
#include "AthenaBaseComps/AthCheckMacros.h"


CostData::CostData() :
  m_costCollection(nullptr),
  m_algTotalTime(0.),
  m_eventLiveTime(1.) {
}


StatusCode CostData::set(const xAOD::TrigCompositeContainer* costCollection) {
  m_costCollection = costCollection;
  ATH_CHECK(cache());
  return StatusCode::SUCCESS;
}


StatusCode CostData::cache() {
  for (const xAOD::TrigComposite* tc : costCollection()) {
    m_algTotalTime += (tc->getDetail<uint64_t>("stop") - tc->getDetail<uint64_t>("start"));
  }
  return StatusCode::SUCCESS;
}


void CostData::setEventLivetime(float time) {
  m_eventLiveTime = time;
}


const xAOD::TrigCompositeContainer& CostData::costCollection() const {
  if (!m_costCollection) {
    throw std::runtime_error("nullptr in CostData::costCollection(). Make sure CostData::set() is called.");
  }
  return *m_costCollection;
}


float CostData::algTotalTimeMilliSec() const {
  return m_algTotalTime * 1e-3; // microseconds to milliseconds
}


float CostData::eventLiveTime() const {
  return m_eventLiveTime;
}

