/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * SCT_DCSFloatCondData.h
 * @file header file for data object
 * @author Susumu Oda - 11/08/17
 **/

#ifndef SCT_DCSFLOATCONDDATA_H
#define SCT_DCSFLOATCONDDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include <map>

class SCT_DCSFloatCondData {
public:
  //constructor
  SCT_DCSFloatCondData();

  //destructor
  virtual ~SCT_DCSFloatCondData() = default;
  //@name main methods
  //@{
  /// set a float value for a channel
  void setValue(const CondAttrListCollection::ChanNum& chanNum, const float value);
  /// get the float value for a channel
  bool getValue(const CondAttrListCollection::ChanNum& chanNum, float& value) const;
  /// clear the m_channelValues
  void clear();
  //@}
  
private:
  typedef std::map<CondAttrListCollection::ChanNum, float> FloatConditions;
  FloatConditions m_channelValues;
};

CLASS_DEF( SCT_DCSFloatCondData , 234553277 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_DCSFloatCondData, 257878639 );


#endif // SCT_DCSFLOATCONDDATA_H
