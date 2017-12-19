//Dear emacs, this is -*- C++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARSYMCONDITIONSALG_H
#define LARSYMCONDITIONSALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"
#include "LArRawConditions/LArMCSym.h"


template<class MC_t, class SYM_t>
class LArSymConditionsAlg: public AthAlgorithm {
 public:

  LArSymConditionsAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArSymConditionsAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize() {return StatusCode::SUCCESS;}

 private:
  SG::ReadCondHandleKey<MC_t> m_readKey;
  SG::ReadCondHandleKey<LArMCSym> m_mcSymKey;
  SG::WriteCondHandleKey<SYM_t>  m_writeKey;
  ServiceHandle<ICondSvc> m_condSvc;

};

#include "LArSymConditionsAlg.icc"

#include "LArRawConditions/LArRampMC.h"
#include "LArRawConditions/LArRampSym.h"
typedef LArSymConditionsAlg<LArRampMC,LArRampSym> LArRampSymCondAlg; 

#include "LArRawConditions/LAruA2MeVMC.h"
#include "LArRawConditions/LAruA2MeVSym.h"
typedef LArSymConditionsAlg<LAruA2MeVMC,LAruA2MeVSym> LAruA2MeVSymCondAlg;


#include "LArRawConditions/LArDAC2uAMC.h"
#include "LArRawConditions/LArDAC2uASym.h"
typedef LArSymConditionsAlg<LArDAC2uAMC,LArDAC2uASym> LArDAC2uASymCondAlg;

#include "LArRawConditions/LArfSamplMC.h"
#include "LArRawConditions/LArfSamplSym.h"
typedef LArSymConditionsAlg<LArfSamplMC,LArfSamplSym> LArfSamplSymCondAlg;



#include "LArRawConditions/LArMinBiasMC.h"
#include "LArRawConditions/LArMinBiasSym.h"
typedef LArSymConditionsAlg<LArMinBiasMC,LArMinBiasSym> LArMinBiasSymCondAlg;

#include "LArRawConditions/LArNoiseMC.h"
#include "LArRawConditions/LArNoiseSym.h"
typedef LArSymConditionsAlg<LArNoiseMC,LArNoiseSym> LArNoiseSymCondAlg;


#endif
