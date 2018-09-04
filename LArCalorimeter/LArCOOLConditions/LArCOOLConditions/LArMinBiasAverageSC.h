/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef LARCOOLCONDITIONS_LARMINBIASAVERAGESC_H
#define LARCOOLCONDITIONS_LARMINBIASAVERAGESC_H

#include "LArElecCalib/ILArMinBiasAverage.h" 
#include "LArCOOLConditions/LArSingleFloatBlob.h"
#include "LArCOOLConditions/LArCondSuperCellBase.h"


class CondAttrListCollection;

class LArMinBiasAverageSC: public ILArMinBiasAverage,
                          public LArCondSuperCellBase,
		          public LArSingleFloatBlob {

public:
  LArMinBiasAverageSC(); 
  LArMinBiasAverageSC(const CondAttrListCollection* attrList);

  virtual ~LArMinBiasAverageSC();

  bool good() const { return m_isInitialized && m_nChannels>0; }
  
  // retrieving LArMinBiasAverage using online ID  
  virtual const float& minBiasAverage(const HWIdentifier& chid) const;

  // retrieving LArMinBias using offline ID  
  virtual const float& minBiasAverage(const Identifier& id) const; 
  
private:
  //static const float errorcode;

};
CLASS_DEF( LArMinBiasAverageSC , 265616076 , 1 )

#endif 
