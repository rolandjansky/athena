/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_MajorityCondData.h
 * header file for data object
 * @author Susumu Oda - 2017-10-13
 **/

#ifndef SCT_MAJORITYCONDDATA_H
#define SCT_MAJORITYCONDDATA_H

#include <map>

// Include Athena stuff
#include "AthenaKernel/CLASS_DEF.h"

class SCT_MajorityCondData {
public:
  // Constructor
  SCT_MajorityCondData();
  // Destructor
  virtual ~SCT_MajorityCondData();

  // Set majority state for a region
  void setMajorityState(const int& region, const bool& majorityState);
  // Get majority state for a region
  bool getMajorityState(const int& region) const;
  // Clear majority state
  void clearMajorityStates();

  // Set HV fraction for a region
  void setHVFraction(const int& region, const float& hvFraction);
  // Get HV fraction for a region
  float getHVFraction(const int& region) const;
  // Clear HV fractions
  void clearHVFractions();

  // Set filled variable
  void setFilled(const bool& filled);
  // Get filled variable
  bool isFilled() const;

private:
  // Map to store majority state
  std::map<int, bool> m_majorityState;
  // Map to store HV fraction
  std::map<int, float> m_hvFraction;
  // Flag to check data are filled or not
  bool m_filled;
};

CLASS_DEF( SCT_MajorityCondData , 131714728 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_MajorityCondData, 80083480 );

#endif // SCT_MAJORITYDATA_H
