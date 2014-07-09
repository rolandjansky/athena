/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARDSPTHRESHOLDSFILLINGALG_H
#define LARDSPTHRESHOLDSFILLINGALG_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloIdentifier/CaloCellGroup.h"
#include "LArTools/LArCablingService.h"
#include "LArRecConditions/ILArBadChannelMasker.h"

class StoreGateSvc;
class LArOnlineID;
class LArCablingService;
class ICaloNoiseTool;
class ILArBadChannelMasker;

class LArDSPThresholdFillAlg:public Algorithm {
 public:
  LArDSPThresholdFillAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArDSPThresholdFillAlg();
  StatusCode initialize();
  StatusCode execute() {return StatusCode::SUCCESS;}
  StatusCode stop();

 private:
  StoreGateSvc* m_detStore;
  const LArOnlineID* m_onlineID;

  std::string m_key;
  std::string m_groupingType;
  std::string m_mode;
  std::vector<std::string> m_cellGroupStr;
  float m_tqThrsh, m_samplesThrsh;
  float m_maskedtqThrsh, m_maskedsamplesThrsh;
  float m_sigmaNoiseSamples, m_sigmaNoiseQt;
  bool m_usePileupNoiseSamples, m_usePileupNoiseQt;
  bool m_fill, m_dump, m_maskBadChannels;
  std::string m_outFileName, m_inFileName;

  ToolHandle < ICaloNoiseTool > m_noisetool;
  ToolHandle < LArCablingService > m_cablingSvc;
  ToolHandle < ILArBadChannelMasker > m_badChannelMasker;

  enum mode_t{
    FIXED,GROUP,NOISE
  };

  mode_t m_workmode;

  CaloCellGroupList m_thrPerCell;
};

#endif
