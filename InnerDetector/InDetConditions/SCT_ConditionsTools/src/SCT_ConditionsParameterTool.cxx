/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConditionsParameterTool.cxx
 * implementation file for SCT conditions tool giving numerical parameters
 * @author shaun.roe@cern.ch
 **/
#include <limits>

#include "SCT_ConditionsParameterTool.h"

//c'tor
SCT_ConditionsParameterTool::SCT_ConditionsParameterTool(const std::string &type, const std::string &name, const IInterface *parent) :
  base_class(type, name, parent) {
}

//
StatusCode
SCT_ConditionsParameterTool::initialize() {
  ATH_CHECK(m_condKey.initialize());

  return StatusCode::SUCCESS;
}

//
StatusCode
SCT_ConditionsParameterTool::finalize() {
  return StatusCode::SUCCESS;
}

///Is the required parameter available?
bool
SCT_ConditionsParameterTool::available(const SCT_CondParameterData::ParameterIndex iparam, const EventContext& /*ctx*/) const {
  return (iparam==SCT_CondParameterData::AVG_THRESHOLD);
}

bool
SCT_ConditionsParameterTool::available(const SCT_CondParameterData::ParameterIndex iparam) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return available(iparam, ctx);
}

///Give the indicated value for a module identifier hash
float
SCT_ConditionsParameterTool::value(const IdentifierHash& idHash, const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const {
  const SCT_CondParameterData* data{getCondData(ctx)};
  if (data==nullptr) return invalid(iparam);
  return data->getValue(idHash, iparam);
}

float
SCT_ConditionsParameterTool::value(const IdentifierHash& idHash, const SCT_CondParameterData::ParameterIndex iparam) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return value(idHash, iparam, ctx);
}

///Measure of how many valid values went to calculate it. Should be 1 but if, say, 3 chip values were valid out of 6, it could be less (0.5 in this case)
float
SCT_ConditionsParameterTool::validity(const IdentifierHash& /*idHash*/, const SCT_CondParameterData::ParameterIndex /*iparam*/, const EventContext& /*ctx*/) const {
  return 1.0;
}

float
SCT_ConditionsParameterTool::validity(const IdentifierHash& idHash, const SCT_CondParameterData::ParameterIndex iparam) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return validity(idHash, iparam, ctx);
}

///Is a given value within acceptable limits?
bool 
SCT_ConditionsParameterTool::isValid(const float parameterValue, const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const {
  const SCT_CondParameterData* data{getCondData(ctx)};
  if (data==nullptr) return invalid(iparam);
  return data->isValid(parameterValue, iparam);
}

bool 
SCT_ConditionsParameterTool::isValid(const float parameterValue, const SCT_CondParameterData::ParameterIndex iparam) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return isValid(parameterValue, iparam, ctx);
}

///What is the default error value for this parameter?
float
SCT_ConditionsParameterTool::invalid(const unsigned int iparam, const EventContext& /*ctx*/) const {
  float result{0.0};
  if (iparam==SCT_CondParameterData::AVG_THRESHOLD) {
    result=std::numeric_limits<float>::quiet_NaN();
  }
  return result;
}

float
SCT_ConditionsParameterTool::invalid(const unsigned int iparam) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return invalid(iparam, ctx);
}

///Maximum value read in from the database
float
SCT_ConditionsParameterTool::max(const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const {
  const SCT_CondParameterData* data{getCondData(ctx)};
  if (data==nullptr) return invalid(iparam);
  return data->max(iparam);
}

float
SCT_ConditionsParameterTool::max(const SCT_CondParameterData::ParameterIndex iparam) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return max(iparam, ctx);
}

///Minimum value read in from the database
float
SCT_ConditionsParameterTool::min(const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const {
  const SCT_CondParameterData* data{getCondData(ctx)};
  if (data==nullptr) return invalid(iparam);
  return data->min(iparam);
}

float
SCT_ConditionsParameterTool::min(const SCT_CondParameterData::ParameterIndex iparam) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return min(iparam, ctx);
}

///Average value
float
SCT_ConditionsParameterTool::avg(const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const {
  const SCT_CondParameterData* data{getCondData(ctx)};
  if (data==nullptr) return invalid(iparam);
  return data->avg(iparam);
}

float
SCT_ConditionsParameterTool::avg(const SCT_CondParameterData::ParameterIndex iparam) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return avg(iparam, ctx);
}

///Standard deviation
float
SCT_ConditionsParameterTool::sd(const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const {
  const SCT_CondParameterData* data{getCondData(ctx)};
  if (data==nullptr) return invalid(iparam);
  return data->sd(iparam);
}

float
SCT_ConditionsParameterTool::sd(const SCT_CondParameterData::ParameterIndex iparam) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return sd(iparam, ctx);
}

///Number of values read in
unsigned int
SCT_ConditionsParameterTool::n(const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const {
  const SCT_CondParameterData* data{getCondData(ctx)};
  if (data==nullptr) return 0;
  return data->n(iparam);
}

unsigned int
SCT_ConditionsParameterTool::n(const SCT_CondParameterData::ParameterIndex iparam) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return n(iparam, ctx);
}

///Fill a user-provided vector with the values (hopefully won't be needed?)
void
SCT_ConditionsParameterTool::getValues(std::vector<float>& userVector, const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const {
  const SCT_CondParameterData* data{getCondData(ctx)};
  if (data!=nullptr) data->getValues(userVector, iparam);
  //no return value
}

void
SCT_ConditionsParameterTool::getValues(std::vector<float>& userVector, const SCT_CondParameterData::ParameterIndex iparam) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  getValues(userVector, iparam, ctx);
}

///Report whether the structure was filled
bool
SCT_ConditionsParameterTool::filled(const EventContext& ctx) const {
  const SCT_CondParameterData* data{getCondData(ctx)};
  return (data!=nullptr);
}

bool
SCT_ConditionsParameterTool::filled() const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return filled(ctx);
}

const SCT_CondParameterData* SCT_ConditionsParameterTool::getCondData(const EventContext& ctx) const {
  SG::ReadCondHandle<SCT_CondParameterData> condData{m_condKey, ctx};
  if (not condData.isValid()) {
    ATH_MSG_ERROR("Failed to get " << m_condKey.key());
    return nullptr;
  }
  return *condData;
}
