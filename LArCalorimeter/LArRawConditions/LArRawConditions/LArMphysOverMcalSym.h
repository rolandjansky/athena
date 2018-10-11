/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARMPHYSOVERMCALSYM_H
#define LARRAWCONDITIONS_LARMPHYSOVERMCALSYM_H

#include "LArElecCalib/ILArMphysOverMcal.h"

class LArMCSym;
class LArMphysOverMcalMC;
class LArSingleFloatP;
template<typename LArSingleFloatP> class LArConditionsContainer;

class LArMphysOverMcalSym: virtual public ILArMphysOverMcal {
 public:

  LArMphysOverMcalSym() = delete;
  LArMphysOverMcalSym(const LArMCSym* mcsym,  const LArMphysOverMcalMC* MphysOverMcalComplete);
  virtual ~LArMphysOverMcalSym( );
  
  virtual const float& MphysOverMcal(const HWIdentifier& hwid, int gain) const override;

 private: 
  const LArMCSym* m_mcSym;
  const LArConditionsContainer<LArSingleFloatP>* m_MphysOverMcalComplete;
};

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArMphysOverMcalSym,130141195,1)
CONDCONT_DEF( LArMphysOverMcalSym, 146726487, ILArMphysOverMcal );
#endif 
