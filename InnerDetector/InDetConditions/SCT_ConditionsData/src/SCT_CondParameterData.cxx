/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_ConditionsData/SCT_CondParameterData.h"

#include <limits>
#include <cmath>

// constructor
SCT_CondParameterData::SCT_CondParameterData() {
  clear();
}

// Get the indicated value for a module identifier hash
float SCT_CondParameterData::getValue(const IdentifierHash& idHash, const SCT_CondParameterData::ParameterIndex iparam) const {
  return m_values[idHash][iparam];
}

// Fill a user-provided vector with the values
void SCT_CondParameterData::getValues(std::vector<float>& userVector, const SCT_CondParameterData::ParameterIndex iparam) const {
  for (unsigned int i{0}; i!=m_values.size(); ++i) {
    userVector.push_back(m_values[i][iparam]);
  }
}

/// Get the indicated value for a module identifier hash
void SCT_CondParameterData::setValue(const IdentifierHash& idHash, const SCT_CondParameterData::ParameterIndex iparam, const float value) {
  if (insert(idHash, iparam, value)) {
    m_n[iparam]++;
    m_sum[iparam] += value;
    m_sumsq[iparam] += value*value;
    m_min[iparam] = std::min(m_min[iparam], value);
    m_max[iparam] = std::max(m_max[iparam], value);
  }
}

// Extended methods for data structure insertion
bool SCT_CondParameterData::insert(const IdentifierHash& idHash, const SCT_CondParameterData::ParameterIndex iparam, const float theValue) {
  // theValue must be valid
  if (not isValid(theValue, iparam)) return false;
  boost::array<float,N_PARAMETERS>& paramArray = m_values[idHash];
  // initial value should be invalid, only insert if this is the case
  if (not isValid(paramArray[iparam], iparam)) {
    paramArray[iparam] = theValue;
    return true;
  } else {
    return false; 
  }
}

// Is a given value within acceptable limits?
bool SCT_CondParameterData::isValid(const float parameterValue, const SCT_CondParameterData::ParameterIndex iparam) const {
  // Second condition is to check if it is nan.
  if (iparam==AVG_THRESHOLD and (parameterValue!=parameterValue)) return false;
  return true;
}

// What is the default error value for this parameter?
float SCT_CondParameterData::invalid(const unsigned int iparam) const {
  float result{0.0};
  if (iparam==AVG_THRESHOLD) {
    result=std::numeric_limits<float>::quiet_NaN();
  }
  return result;
}

// Get maximum value
float SCT_CondParameterData::max(const SCT_CondParameterData::ParameterIndex iparam) const {
  return m_max[iparam];
}

// Get minimum value
float SCT_CondParameterData::min(const SCT_CondParameterData::ParameterIndex iparam) const {
  return m_min[iparam];
}

// Get average value
float SCT_CondParameterData::avg(const SCT_CondParameterData::ParameterIndex iparam) const {
  return (m_n[iparam]!=0)?(m_sum[iparam]/m_n[iparam]):(std::numeric_limits<float>::quiet_NaN());
}

// Get standard deviation
float SCT_CondParameterData::sd(const SCT_CondParameterData::ParameterIndex iparam) const {
  float mu{m_sum[iparam]/m_n[iparam]};
  return std::sqrt((m_sumsq[iparam]/m_n[iparam]) - mu*mu);
}

// Get the number of values
unsigned int SCT_CondParameterData::n(const SCT_CondParameterData::ParameterIndex iparam) const {
  return m_n[iparam];
}

// Clear all data members
void SCT_CondParameterData::clear() {
  boost::array<float, N_PARAMETERS> init;
  for (unsigned int i{0}; i!=N_PARAMETERS; ++i) {
    m_min[i] = std::numeric_limits<float>::max();
    m_max[i] = std::numeric_limits<float>::min();
    m_n[i] = 0;
    m_sum[i] = 0.0;
    m_sumsq[i] = 0.0;
    init[i] = invalid(i);
  }
  
  //initialize boost array
  for (unsigned int i{0}; i!=N_ELEMENTS; ++i) {
    m_values[i] = init;
  }
}
