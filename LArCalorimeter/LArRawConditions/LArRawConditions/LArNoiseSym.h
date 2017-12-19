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

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArNoiseSym,235778925,1)
CONDCONT_DEF( LArNoiseSym, 233925277, ILArNoise );
#endif 
