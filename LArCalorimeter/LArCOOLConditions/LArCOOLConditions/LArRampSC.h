/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARCOOLCONDITIONS_LARRAMPSUPERCELL_H
#define LARCOOLCONDITIONS_LARRAMPSUPERCELL_H

#include "LArElecCalib/ILArRamp.h" 
#include "LArCOOLConditions/LArCondSuperCellBase.h"
#include "LArCOOLConditions/LArRampBlob.h"

class CondAttrListCollection;

class LArRampSC: 
  public ILArRamp, public LArCondSuperCellBase, public LArRampBlob
{  
 private:
  LArRampSC();

 public:  
  LArRampSC(const CondAttrListCollection* attrList);
  virtual ~LArRampSC( );

  bool good() const { return m_isInitialized && m_nChannels>0; }

  virtual  RampRef_t ADC2DAC(const HWIdentifier&  CellID, int gain) const ;  
};
#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArRampSC , 43028438, 1 )
CONDCONT_DEF( LArRampSC, 147380444, ILArRamp );


#endif 
