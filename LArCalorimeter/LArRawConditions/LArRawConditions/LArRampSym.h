/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARRAMPSYM_H
#define LARRAWCONDITIONS_LARRAMPSYM_H

#include "LArElecCalib/ILArRamp.h"


class LArMCSym;
class LArRampMC;
class LArRampP1;
template <typename LArRampP1> class LArConditionsContainer;

class LArRampSym: public ILArRamp {
 public:
  LArRampSym() = delete;
  LArRampSym(const LArMCSym* mcsym,  const LArRampMC* rampComplete);
  virtual ~LArRampSym( );
  
  virtual RampRef_t ADC2DAC(const HWIdentifier& CellID, int gain) const override;

 private: 
  const LArMCSym* m_mcSym;
  const LArConditionsContainer<LArRampP1>* m_rampComplete;
};

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArRampSym,226012704,1)
CONDCONT_DEF( LArRampSym, 121227424, ILArRamp );

#endif 
