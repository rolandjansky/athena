/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConditionsParameterSvc.cxx
 * implementation file for SCT conditions service giving numerical parameters
 * @author shaun.roe@cern.ch
 **/
#include <limits>

#include "SCT_ConditionsParameterSvc.h"
#include "SCT_Chip.h"

//c'tor
SCT_ConditionsParameterSvc::SCT_ConditionsParameterSvc(const std::string& name, ISvcLocator* pSvcLocator):
  AthService(name, pSvcLocator),
  m_condKey{"SCT_CondParameterData"} {
  }

//
StatusCode
SCT_ConditionsParameterSvc::initialize() {
  ATH_CHECK(m_condKey.initialize());

  return StatusCode::SUCCESS;
}

//
StatusCode
SCT_ConditionsParameterSvc::finalize() {
  return StatusCode::SUCCESS;
}

//
StatusCode
SCT_ConditionsParameterSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (ISCT_ConditionsParameterSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = this;
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

///Is the required parameter available?
bool
SCT_ConditionsParameterSvc::available(const SCT_CondParameterData::ParameterIndex iparam) {
  return (iparam==SCT_CondParameterData::AVG_THRESHOLD);
}

///Give the indicated value for a module identifier hash
float
SCT_ConditionsParameterSvc::value(const IdentifierHash& idHash, const SCT_CondParameterData::ParameterIndex iparam) {
  const SCT_CondParameterData* data{getCondData()};
  if (data==nullptr) return invalid(iparam);
  return data->getValue(idHash, iparam);
}

///Measure of how many valid values went to calculate it. Should be 1 but if, say, 3 chip values were valid out of 6, it could be less (0.5 in this case)
float
SCT_ConditionsParameterSvc::validity(const IdentifierHash& /*idHash*/, const SCT_CondParameterData::ParameterIndex /*iparam*/) {
  return 1.0;
}

///Is a given value within acceptable limits?
bool 
SCT_ConditionsParameterSvc::isValid(const float parameterValue, const SCT_CondParameterData::ParameterIndex iparam) {
  const SCT_CondParameterData* data{getCondData()};
  if (data==nullptr) return invalid(iparam);
  return data->isValid(parameterValue, iparam);
}

///What is the default error value for this parameter?
float
SCT_ConditionsParameterSvc::invalid(const unsigned int iparam) const {
  float result{0.0};
  if (iparam==SCT_CondParameterData::AVG_THRESHOLD) {
    result=std::numeric_limits<float>::quiet_NaN();
  }
  return result;
}

///Maximum value read in from the database
float
SCT_ConditionsParameterSvc::max(const SCT_CondParameterData::ParameterIndex iparam) {
  const SCT_CondParameterData* data{getCondData()};
  if (data==nullptr) return invalid(iparam);
  return data->max(iparam);
}

///Minimum value read in from the database
float
SCT_ConditionsParameterSvc::min(const SCT_CondParameterData::ParameterIndex iparam) {
  const SCT_CondParameterData* data{getCondData()};
  if (data==nullptr) return invalid(iparam);
  return data->min(iparam);
}

///Average value
float
SCT_ConditionsParameterSvc::avg(const SCT_CondParameterData::ParameterIndex iparam) {
  const SCT_CondParameterData* data{getCondData()};
  if (data==nullptr) return invalid(iparam);
  return data->avg(iparam);
}

///Standard deviation
float
SCT_ConditionsParameterSvc::sd(const SCT_CondParameterData::ParameterIndex iparam) {
  const SCT_CondParameterData* data{getCondData()};
  if (data==nullptr) return invalid(iparam);
  return data->sd(iparam);
}

///Number of values read in
unsigned int
SCT_ConditionsParameterSvc::n(const SCT_CondParameterData::ParameterIndex iparam) {
  const SCT_CondParameterData* data{getCondData()};
  if (data==nullptr) return 0;
  return data->n(iparam);
}

///Fill a user-provided vector with the values (hopefully won't be needed?)
void
SCT_ConditionsParameterSvc::getValues(std::vector<float>& userVector, const SCT_CondParameterData::ParameterIndex iparam) {
  const SCT_CondParameterData* data{getCondData()};
  if (data!=nullptr) data->getValues(userVector, iparam);
  //no return value
}

///Report whether the structure was filled
bool
SCT_ConditionsParameterSvc::filled() const {
  const SCT_CondParameterData* data{getCondData()};
  return (data!=nullptr);
}

///Callback for fill from database
StatusCode
SCT_ConditionsParameterSvc::fillData(int& /* i */, std::list<std::string>& /*keys*/) {
  return StatusCode::SUCCESS;
}

const SCT_CondParameterData* SCT_ConditionsParameterSvc::getCondData() const {
  SG::ReadCondHandle<SCT_CondParameterData> condData{m_condKey};
  if (not condData.isValid()) {
    ATH_MSG_ERROR("Failed to get " << m_condKey.key());
    return nullptr;
  }
  return *condData;
}
