/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARNOISESYM_H
#define LARRAWCONDITIONS_LARNOISESYM_H

#include "LArElecCalib/ILArNoise.h"

class LArMCSym;
class LArNoiseMC;
class LArNoiseComplete;

class LArNoiseSym: virtual public ILArNoise {
  
 public:

  LArNoiseSym() = delete;

  LArNoiseSym(const LArMCSym* mcsym,  const LArNoiseMC* noiseComplete);
  
  virtual ~LArNoiseSym( );
  
  virtual const float& noise(const HWIdentifier& hwid, int gain) const override;
  
  virtual const float& noise(const Identifier& CellID, int gain) const override;
  
 private: 

  const LArMCSym* m_mcSym;
  const LArNoiseComplete* m_noiseComplete;

};

#include "CLIDSvc/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
CONDCONT_BASE(LArNoiseSym, ILArNoise);
CLASS_DEF( LArNoiseSym,235778925,1)
CLASS_DEF( CondCont<LArNoiseSym> , 233925277, 1 )
#endif 
