/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef LARCOOLCONDITIONS_LARHVSCALECORR_H
#define LARCOOLCONDITIONS_LARHVSCALECORR_H

#include "LArElecCalib/ILArHVScaleCorr.h" 
#include "LArCOOLConditions/LArSingleFloatBlob.h"
#include "LArCOOLConditions/LArCondSuperCellBase.h"


class CondAttrListCollection;

class LArHVScaleCorrSC: public ILArHVScaleCorr,
                        public LArCondSuperCellBase,
			public LArSingleFloatBlob {

public:
  LArHVScaleCorrSC(); 
  LArHVScaleCorrSC(const CondAttrListCollection* attrList);

  virtual ~LArHVScaleCorrSC();

  bool good() const { return m_isInitialized && m_nChannels>0; }
  
  // retrieving LArHVScaleCorr using online ID  
  virtual const float& HVScaleCorr(const HWIdentifier& chid) const;

  // retrieving LArHVScaleCorr using offline ID  
  virtual const float& HVScaleCorr(const Identifier& id) const; 

private:
  //static const float errorcode;

};
#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArHVScaleCorrSC ,9991567 , 1 )
CONDCONT_DEF( LArHVScaleCorrSC, 123730893, ILArHVScaleCorr );

#endif 
