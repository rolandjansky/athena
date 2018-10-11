/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARMIBIASSYM_H
#define LARRAWCONDITIONS_LARMIBIASSYM_H

#include "LArElecCalib/ILArMinBias.h"

class LArMCSym;
class LArMinBiasMC;
class LArMinBiasP;
template<typename LArMinBiastP> class LArConditionsContainer;


class LArMinBiasSym: virtual public ILArMinBias {
 public:
  LArMinBiasSym() = delete;
  LArMinBiasSym(const LArMCSym* mcsym,  const LArMinBiasMC* MinBiasComplete);
  virtual ~LArMinBiasSym( );

  virtual const float& minBiasRMS(const HWIdentifier& hwid) const override;
  
 private: 
  const LArMCSym* m_mcSym;
  const LArConditionsContainer<LArMinBiasP>* m_MinBiasComplete;
};

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArMinBiasSym,211635936,1)
CONDCONT_DEF( LArMinBiasSym ,185039882, ILArMinBias );
#endif 
