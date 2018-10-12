//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARAUTOCORRSYM_H
#define LARRAWCONDITIONS_LARAUTOCORRSYM_H

#include "LArElecCalib/ILArAutoCorr.h"

class LArMCSym;
class LArAutoCorrMC;
class LArAutoCorrP1;
template<typename LArAutoCorrP1> class LArConditionsContainer; 


class LArAutoCorrSym: public ILArAutoCorr {  
 public:
  LArAutoCorrSym() = delete;
  LArAutoCorrSym(const LArMCSym* mcsym, const LArAutoCorrMC* acComplete);
  virtual ~LArAutoCorrSym( );
 
  virtual AutoCorrRef_t autoCorr(const HWIdentifier& CellID, int gain) const override;
   
 private: 
  const LArMCSym* m_mcSym;
  const LArConditionsContainer<LArAutoCorrP1>* m_acComplete;
};

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArAutoCorrSym,137573373,1)
CONDCONT_DEF( LArAutoCorrSym, 183122331, ILArAutoCorr );

#endif 
