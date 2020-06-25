// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_MajorityCondData.h
 * @brief header file for data object for SCT_MajorityCondAlg and SCT_MajorityConditionsTool.
 * @author Susumu Oda
 * @date 2017-10-13
 **/

#ifndef SCT_MAJORITYCONDDATA_H
#define SCT_MAJORITYCONDDATA_H

#include <map>

/**
 * @class SCT_MajorityCondData
 * @brief Class for data object used in SCT_MajorityCondAlg and SCT_MajorityConditionsTool.
 **/
class SCT_MajorityCondData {
public:
  /// Constructor
  SCT_MajorityCondData();
  /// Destructor
  virtual ~SCT_MajorityCondData() = default;

  /// Set majority state for a region
  void setMajorityState(const int& region, const bool& majorityState);
  /// Get majority state for a region
  bool getMajorityState(const int& region) const;
  /// Clear majority state
  void clearMajorityStates();

  /// Set HV fraction for a region
  void setHVFraction(const int& region, const float& hvFraction);
  /// Get HV fraction for a region
  float getHVFraction(const int& region) const;
  /// Clear HV fractions
  void clearHVFractions();

  /// Set filled variable
  void setFilled(const bool& filled);
  /// Get filled variable
  bool isFilled() const;

private:
  /// Map to store majority state
  std::map<int, bool> m_majorityState;
  /// Map to store HV fraction
  std::map<int, float> m_hvFraction;
  /// Flag to check data are filled or not
  bool m_filled;
};

// Class definition for StoreGate
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_MajorityCondData , 131714728 , 1 )

// Condition container definition for CondInputLoader
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_MajorityCondData, 80083480 );

#endif // SCT_MAJORITYDATA_H
