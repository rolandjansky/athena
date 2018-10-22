/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARMINAVERAGEBIASSYM_H
#define LARRAWCONDITIONS_LARMINAVERAGEBIASSYM_H

#include "LArElecCalib/ILArMinBiasAverage.h"


class LArMCSym;
class LArMinBiasAverageMC;
class LArMinBiasAverageP;
template<typename LArMinBiasAverageP> class LArConditionsContainer;

class LArMinBiasAverageSym: virtual public ILArMinBiasAverage {
  
 public:
  LArMinBiasAverageSym() = delete;
  LArMinBiasAverageSym(const LArMCSym* mcsym,  const LArMinBiasAverageMC* MinBiasAverageComplete);
  virtual ~LArMinBiasAverageSym( );
  
  virtual const float& minBiasAverage(const HWIdentifier& hwid) const override;
  
 private: 
  const LArMCSym* m_mcSym;
  const LArConditionsContainer<LArMinBiasAverageP>* m_MinBiasAverageComplete;
};

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArMinBiasAverageSym,14136214 ,1)
CONDCONT_DEF( LArMinBiasAverageSym ,261435098, ILArMinBiasAverage );
#endif 
