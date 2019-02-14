/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * SCT_CondParameterData.h
 * @file header file for data object
 * @author Susumu Oda - 2018-01-25
 **/

#ifndef SCT_CONDPARAMETERDATA_H
#define SCT_CONDPARAMETERDATA_H

// STL include
#include <vector>

// boost include
#include "boost/array.hpp"

// Athena includes
#include "Identifier/IdentifierHash.h"
#include "AthenaKernel/CLASS_DEF.h"

class SCT_CondParameterData {
public:
  // Parameters
  enum ParameterIndex{AVG_THRESHOLD, N_PARAMETERS, INVALID_PARAMETER};

  //constructor
  SCT_CondParameterData();

  //destructor
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
  enum {N_ELEMENTS=8176};

  boost::array<boost::array<float, N_PARAMETERS>, N_ELEMENTS > m_values;

  float m_min[N_PARAMETERS];
  float m_max[N_PARAMETERS];
  unsigned int m_n[N_PARAMETERS];
  float m_sum[N_PARAMETERS];
  float m_sumsq[N_PARAMETERS];
};

CLASS_DEF( SCT_CondParameterData , 34383719 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_CondParameterData, 3856939 );


#endif // SCT_CONDPARAMETERDATA_H
