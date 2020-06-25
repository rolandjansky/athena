// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_DCSFloatCondData.h
 * @brief header file for data object for SCT_DCSConditions{HV,Temp}CondAlg,
 * SCT_DCSConditionsTool, SCT_SiliconConditions{HV,Temp}CondAlg,
 * SCTSiPropertiesCondAlg, SCTSiLorentzAngleCondAlg.
 * @author Susumu Oda
 * @date 11/08/17
 **/

#ifndef SCT_DCSFLOATCONDDATA_H
#define SCT_DCSFLOATCONDDATA_H

#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include <map>

/**
 * @class SCT_DCSFloatCondData
 * @brief Class for data object used in
 * SCT_DCSConditions{HV,Temp}CondAlg,
 * SCT_DCSConditionsTool, SCT_SiliconConditions{HV,Temp}CondAlg,
 * SCTSiPropertiesCondAlg, SCTSiLorentzAngleCondAlg.
 **/
class SCT_DCSFloatCondData {
public:
  /// Constructor
  SCT_DCSFloatCondData();

  /// Destructor
  virtual ~SCT_DCSFloatCondData() = default;
  //@name main methods
  //@{
  /// Set a float value for a channel
  void setValue(const CondAttrListCollection::ChanNum& chanNum, const float value);
  /// Get the float value for a channel
  bool getValue(const CondAttrListCollection::ChanNum& chanNum, float& value) const;
  /// Clear the m_channelValues
  void clear();
  //@}
  
private:
  typedef std::map<CondAttrListCollection::ChanNum, float> FloatConditions;
  FloatConditions m_channelValues;
};

// Class definition for StoreGate
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_DCSFloatCondData , 234553277 , 1 )

// Condition container definition for CondInputLoader
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_DCSFloatCondData, 257878639 );


#endif // SCT_DCSFLOATCONDDATA_H
