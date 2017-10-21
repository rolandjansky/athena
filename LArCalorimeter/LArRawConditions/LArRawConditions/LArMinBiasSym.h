/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARMIBIASSYM_H
#define LARRAWCONDITIONS_LARMIBIASSYM_H

#include "LArElecCalib/ILArMinBias.h"

class LArMCSym;
class LArMinBiasMC;
class LArMinBiasComplete;

class LArMinBiasSym: virtual public ILArMinBias {
  
 public:

  LArMinBiasSym() = delete;

  LArMinBiasSym(const LArMCSym* mcsym,  const LArMinBiasMC* MinBiasComplete);
  
  virtual ~LArMinBiasSym( );
  
  virtual const float& minBiasRMS(const HWIdentifier& hwid) const override;
  
  virtual const float& minBiasRMS(const Identifier& CellID) const override;
  
 private: 

  const LArMCSym* m_mcSym;
  const LArMinBiasComplete* m_MinBiasComplete;

};

#include "CLIDSvc/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
CONDCONT_BASE(LArMinBiasSym, ILArMinBias);
CLASS_DEF( LArMinBiasSym,211635936,1)
CLASS_DEF( CondCont<LArMinBiasSym> ,185039882, 1 )
#endif 
