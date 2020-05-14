// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_CondParameterData.h
 * @brief header file for data object for SCT_ConditionsParameterTool and SCT_ConditionsParameterCondAlg.
 * @author Susumu Oda
 * @date 2018-01-25
 **/

#ifndef SCT_CONDPARAMETERDATA_H
#define SCT_CONDPARAMETERDATA_H

// Definition of the number of elements
#include "SCT_ConditionsData/SCT_ConditionsParameters.h"

// Athena includes
#include "Identifier/IdentifierHash.h"

// STL include
#include <array>
#include <vector>

/**
 * @class SCT_CondParameterData
 * @brief Data object for SCT_ConditionsParameterTool and SCT_ConditionsParameterCondAlg
 **/

class SCT_CondParameterData {
public:
  /// Parameters
  enum ParameterIndex{AVG_THRESHOLD, N_PARAMETERS, INVALID_PARAMETER};

  /// Constructor
  SCT_CondParameterData();

  /// Destructor
  virtual ~SCT_CondParameterData() = default;
  //@name main methods
  //@{
  /// Get the indicated value for a module identifier hash
  float getValue(const IdentifierHash& idHash, const SCT_CondParameterData::ParameterIndex iparam) const;
  /// Fill a user-provided vector with the values
  void getValues(std::vector<float>& userVector, const SCT_CondParameterData::ParameterIndex iparam) const;
  /// Get the indicated value for a module identifier hash
  void setValue(const IdentifierHash& idHash, const SCT_CondParameterData::ParameterIndex iparam, const float value);
  /// Extended methods for data structure insertion
  bool insert(const IdentifierHash& idHash, const SCT_CondParameterData::ParameterIndex iparam, const float value);
  /// Is a given value within acceptable limits?
  bool isValid(const float parameterValue, const SCT_CondParameterData::ParameterIndex iparam) const;
  /// What is the default error value for this parameter?
  float invalid(const unsigned int iparam) const;
  /// Get maximum value
  float max(const SCT_CondParameterData::ParameterIndex iparam) const;
  /// Get minimum value
  float min(const SCT_CondParameterData::ParameterIndex iparam) const;
  /// Get average value
  float avg(const SCT_CondParameterData::ParameterIndex iparam) const;
  /// Get standard deviation
  float sd(const SCT_CondParameterData::ParameterIndex iparam) const;
  /// Get the number of values
  unsigned int n(const SCT_CondParameterData::ParameterIndex iparam) const;
  /// Clear all data members
  void clear();
  //@}
  
private:
  std::array<std::array<float, N_PARAMETERS>, SCT_ConditionsData::NUMBER_OF_WAFERS> m_values;

  float m_min[N_PARAMETERS];
  float m_max[N_PARAMETERS];
  unsigned int m_n[N_PARAMETERS];
  float m_sum[N_PARAMETERS];
  float m_sumsq[N_PARAMETERS];
};

// Class definition for StoreGate
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_CondParameterData , 34383719 , 1 )

// Condition container definition for CondInputLoader
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_CondParameterData, 3856939 );

#endif // SCT_CONDPARAMETERDATA_H
