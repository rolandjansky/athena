/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_SensorsTool.cxx
 * implementation file for SCT conditions tool giving sensors Vdep, crystal orientation and Mfr
 * @author shaun.roe@cern.ch
 **/

#include "SCT_SensorsTool.h"

SCT_SensorsTool::SCT_SensorsTool(const std::string& type, const std::string& name, const IInterface* parent) : 
  base_class(type, name, parent),
  m_mutex{},
  m_cache{},
  m_condData{},
  m_condKey{"SCT_SensorsCondData"} {
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode SCT_SensorsTool::initialize() {
  // Read Cond Handle Key
  ATH_CHECK(m_condKey.initialize());
  
  return StatusCode::SUCCESS;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode SCT_SensorsTool::finalize() {
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

const SCT_SensorCondData* SCT_SensorsTool::getSensorsData(const unsigned int truncatedSerialNumber) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_SensorsCondData* condData{getCondData(ctx)};
  if (condData==nullptr) return nullptr;

  SCT_SensorsCondData::const_iterator it{condData->find(truncatedSerialNumber)};
  if (it!=condData->end()) return &((*it).second);
  return nullptr;
}

void SCT_SensorsTool::getSensorsData(std::vector<std::string>& userVector) {
  ATH_MSG_WARNING("This void SCT_SensorsTool::getSensorsData(std::vector<std::string>& userVector) method is not implemented.");
  for (unsigned int i{0};i!=m_sensorsValues.size(); ++i) {
    userVector.push_back(m_sensorsValues[i]);
  }
}

std::string SCT_SensorsTool::getManufacturer(unsigned int truncatedSerialNumber) {
  std::string manufacturer{""};
  
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_SensorsCondData* condData{getCondData(ctx)};
  if (condData==nullptr) return manufacturer;

  SCT_SensorsCondData::const_iterator it{condData->find(truncatedSerialNumber)};
  if (it!=condData->end()) {
    manufacturer = (*it).second.getManufacturer();
  }
  return manufacturer;
}

void SCT_SensorsTool::printManufacturers() {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_SensorsCondData* condData{getCondData(ctx)};
  if (condData==nullptr) return;

  for(auto it: *condData) {
    ATH_MSG_ALWAYS("channel " << it.first << " manufacturer " << (it.second).getManufacturer());
  }
}

const SCT_SensorsCondData*
SCT_SensorsTool::getCondData(const EventContext& ctx) const {
  static const EventContext::ContextEvt_t invalidValue{EventContext::INVALID_CONTEXT_EVT};
  EventContext::ContextID_t slot{ctx.slot()};
  EventContext::ContextEvt_t evt{ctx.evt()};
  std::lock_guard<std::mutex> lock{m_mutex};
  if (slot>=m_cache.size()) {
    m_cache.resize(slot+1, invalidValue); // Store invalid values in order to go to the next IF statement.
  }
  if (m_cache[slot]!=evt) {
    SG::ReadCondHandle<SCT_SensorsCondData> condData{m_condKey};
    if (not condData.isValid()) {
      ATH_MSG_ERROR("Failed to get " << m_condKey.key());
    }
    m_condData.set(*condData);
    m_cache[slot] = evt;
  }
  return m_condData.get();
}
