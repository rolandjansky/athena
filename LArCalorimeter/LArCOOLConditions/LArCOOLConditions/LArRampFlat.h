/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARCOOLCONDITIONS_LARRAMPFLAT_H
#define LARCOOLCONDITIONS_LARRAMPFLAT_H

#include "LArElecCalib/ILArRamp.h" 
#include "LArCOOLConditions/LArCondFlatBase.h"
#include "LArCOOLConditions/LArRampBlob.h"
#include <vector>

class CondAttrListCollection;

class LArRampFlat: 
  public ILArRamp, public LArCondFlatBase, public LArRampBlob
{  
 private:
  LArRampFlat();

 public:  
  LArRampFlat(const CondAttrListCollection* attrList);
  virtual ~LArRampFlat( );

  bool good() const { return m_isInitialized && m_nChannels>0; }

  virtual  RampRef_t ADC2DAC(const HWIdentifier&  CellID, int gain) const ;  
  virtual  RampRef_t ADC2DAC(const Identifier&  CellID, int gain) const;
  

};

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArRampFlat , 228930547 , 1 )
CONDCONT_DEF( LArRampFlat, 218808135, ILArRamp );


#endif 
