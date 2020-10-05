//Dear emacs, this is -*- C++ -*- 

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  
  //delegate to base-class ctor
  using AthAlgorithm::AthAlgorithm;

  virtual ~LArSymConditionsAlg()=default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override {return StatusCode::SUCCESS;}

 private:
  SG::ReadCondHandleKey<LArMCSym> m_mcSymKey{this,"LArMCSym","LArMCSym","Key of the LArMCSym symmetry table CDO"};
  SG::ReadCondHandleKey<MC_t> m_readKey{this,"ReadKey","LArRampMC","Key of LArXYZMC input CDO"}; 
  SG::WriteCondHandleKey<SYM_t>  m_writeKey{this,"WriteKey","RampSym","Key of LArXYZSym output CDO"};
  ServiceHandle<ICondSvc> m_condSvc{this,"CondSvc","CondSvc"};

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


#include "LArRawConditions/LArMinBiasAverageMC.h"
#include "LArRawConditions/LArMinBiasAverageSym.h"
typedef  LArSymConditionsAlg<LArMinBiasAverageMC,LArMinBiasAverageSym> LArMinBiasAverageSymCondAlg;

#include "LArRawConditions/LArShape32MC.h"
#include "LArRawConditions/LArShape32Sym.h"
typedef LArSymConditionsAlg<LArShape32MC,LArShape32Sym> LArShapeSymCondAlg;

#include "LArRawConditions/LArAutoCorrMC.h"
#include "LArRawConditions/LArAutoCorrSym.h"
typedef LArSymConditionsAlg<LArAutoCorrMC,LArAutoCorrSym> LArAutoCorrSymCondAlg;

#include "LArRawConditions/LArMphysOverMcalMC.h"
#include "LArRawConditions/LArMphysOverMcalSym.h"
typedef LArSymConditionsAlg<LArMphysOverMcalMC,LArMphysOverMcalSym> LArMphysOverMcalSymCondAlg;

#endif
