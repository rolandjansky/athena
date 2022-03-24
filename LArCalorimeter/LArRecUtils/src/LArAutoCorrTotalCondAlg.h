/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARAUTOCORRTOTALCONDALG_H
#define LARAUTOCORRTOTALCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "LArRawConditions/LArADC2MeV.h"
#include "LArRawConditions/LArAutoCorrTotal.h"

#include "LArCabling/LArOnOffIdMapping.h"

#include "LArElecCalib/ILArShape.h"
#include "LArElecCalib/ILArAutoCorr.h"
#include "LArElecCalib/ILArNoise.h"
#include "LArElecCalib/ILArPedestal.h"
#include "LArElecCalib/ILArfSampl.h"
#include "LArElecCalib/ILArMinBias.h"

class LArAutoCorrTotal;

class LArAutoCorrTotalCondAlg : public AthAlgorithm {
public:
  LArAutoCorrTotalCondAlg(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~LArAutoCorrTotalCondAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

private:

  SG::ReadCondHandleKey<LArADC2MeV> m_LArADC2MeVObjKey;

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_LArOnOffIdMappingObjKey;

  SG::ReadCondHandleKey<ILArShape> m_LArShapeObjKey;
  SG::ReadCondHandleKey<ILArAutoCorr> m_LArAutoCorrObjKey;
  SG::ReadCondHandleKey<ILArNoise> m_LArNoiseObjKey;
  SG::ReadCondHandleKey<ILArPedestal> m_LArPedestalObjKey;
  SG::ReadCondHandleKey<ILArfSampl> m_LArfSamplObjKey;
  SG::ReadCondHandleKey<ILArMinBias> m_LArMinBiasObjKey;

  SG::WriteCondHandleKey<LArAutoCorrTotal> m_LArAutoCorrTotalObjKey;

  float m_Nminbias;
  bool m_NoPile;
  bool m_isMC;
  bool m_isSuperCell;
  int m_Nsamples;
  unsigned int m_firstSample;
  int m_deltaBunch;

  size_t m_nGains = 0UL;
};

#endif
