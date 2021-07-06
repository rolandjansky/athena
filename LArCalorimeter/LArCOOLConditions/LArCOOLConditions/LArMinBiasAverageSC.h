/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
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

private:
  //static const float errorcode;

};
#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArMinBiasAverageSC , 265616076 , 1 )
CONDCONT_DEF( LArMinBiasAverageSC, 190001154, ILArMinBiasAverage );

#endif 

