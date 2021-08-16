/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARAUTOCORRNOISECONDALG_H
#define LARAUTOCORRNOISECONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ICondSvc.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRawConditions/LArMCSym.h"

#include "LArElecCalib/ILArAutoCorr.h"

#include "LArRawConditions/LArAutoCorrNoise.h"

class LArAutoCorrNoise;

class LArAutoCorrNoiseCondAlg : public AthAlgorithm
{
public:
  LArAutoCorrNoiseCondAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~LArAutoCorrNoiseCondAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

private:
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_LArOnOffIdMappingObjKey;
  SG::ReadCondHandleKey<LArMCSym>          m_LArMCSymObjKey;

  SG::ReadCondHandleKey<ILArAutoCorr> m_LArAutoCorrObjKey;

  SG::WriteCondHandleKey<LArAutoCorrNoise> m_LArAutoCorrNoiseObjKey;

  ServiceHandle<ICondSvc> m_condSvc;

  bool m_isSuperCell;
  bool m_MCsym;

  unsigned int m_nGains = 0U;
  int m_nSampl;
};

#endif
