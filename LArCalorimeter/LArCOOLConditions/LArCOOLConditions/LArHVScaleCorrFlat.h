/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef LARCOOLCONDITIONS_LARHVSCALECORRFLAT_H
#define LARCOOLCONDITIONS_LARHVSCALECORRFLAT_H

#include "LArElecCalib/ILArHVScaleCorr.h" 
#include "LArCOOLConditions/LArSingleFloatBlob.h"
#include "LArCOOLConditions/LArCondFlatBase.h"

class CondAttrListCollection;

class LArHVScaleCorrFlat: public ILArHVScaleCorr,
			  public LArCondFlatBase,
			  public LArSingleFloatBlob {
private:
  LArHVScaleCorrFlat(); 

public:
  LArHVScaleCorrFlat(const CondAttrListCollection* attrList);
  virtual ~LArHVScaleCorrFlat();
  
  bool good() const { return m_isInitialized && m_nChannels>0; }

  // retrieving HVScaleCorr using online ID  
  virtual const float& HVScaleCorr(const HWIdentifier& chid) const;

  // retrieving HVScaleCorr using offline ID  
  virtual const float& HVScaleCorr(const Identifier& id) const; 
  
};

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArHVScaleCorrFlat , 245481026 , 1 )
CONDCONT_DEF( LArHVScaleCorrFlat, 11793039, ILArHVScaleCorr );
#endif 
