/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARDSPTHRESHOLDSFILLINGINLINE_H
#define LARDSPTHRESHOLDSFILLINGINLINE_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloIdentifier/CaloCellGroup.h"
#include "LArRecConditions/ILArBadChannelMasker.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "CaloConditions/CaloNoise.h"

class LArOnlineID;
class ILArBadChannelMasker;

class LArDSPThresholdFillInline:public AthAlgorithm {
 public:
  LArDSPThresholdFillInline(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~LArDSPThresholdFillInline();
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override {return StatusCode::SUCCESS;}
  virtual StatusCode stop() override;

 private:
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  const LArOnlineID* m_onlineID;

  std::string m_nameOfSet;
  std::string m_key;
  std::string m_mode;
  std::vector<std::string> m_cellGroupStr;
  float m_tqThrsh, m_samplesThrsh;
  float m_maskedtqThrsh, m_maskedsamplesThrsh;
  float m_sigmaNoiseSamples, m_sigmaNoiseQt;
  bool m_usePileupNoiseSamples, m_usePileupNoiseQt;
  bool m_dump, m_maskBadChannels;
  std::string m_outFileName, m_inFileName;
  bool m_fill;

  SG::ReadCondHandleKey<CaloNoise> m_totalNoiseKey
    { this, "TotalNoiseKey", "totalNoise", "SG key for total noise" };
  SG::ReadCondHandleKey<CaloNoise> m_elecNoiseKey
    { this, "ElecNoiseKey", "electronicNoise", "SG key for electronic noise" };

  ToolHandle < ILArBadChannelMasker > m_badChannelMasker;

  enum mode_t{
    FIXED,GROUP,NOISE
  };

  mode_t m_workmode;

  CaloCellGroupList m_thrPerCell;
};

#endif
