/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARDAC2UASYM_H
#define LARRAWCONDITIONS_LARDAC2UASYM_H

#include "LArElecCalib/ILArDAC2uA.h"

class LArMCSym;
class LArDAC2uAMC;
class LArSingleFloatP;
template<typename LArSingleFloatP> class LArConditionsContainer;


class LArDAC2uASym: virtual public ILArDAC2uA {
 public:
  LArDAC2uASym() = delete;
  LArDAC2uASym(const LArMCSym* mcsym,  const LArDAC2uAMC* DAC2uAComplete);
  virtual ~LArDAC2uASym( );
  
  virtual const float& DAC2UA(const HWIdentifier& hwid) const override;
  
 private: 
  const LArMCSym* m_mcSym;
  const LArConditionsContainer<LArSingleFloatP> *m_DAC2uAComplete;
};

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArDAC2uASym,235307622,1)
CONDCONT_DEF( LArDAC2uASym, 22175210, ILArDAC2uA );
#endif 
