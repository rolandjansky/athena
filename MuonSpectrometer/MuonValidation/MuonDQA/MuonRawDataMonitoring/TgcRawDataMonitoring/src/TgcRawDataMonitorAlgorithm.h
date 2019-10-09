/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCRAWDATAMONITORING_TGCRAWDATAMONITORALGORITHM_H
#define TGCRAWDATAMONITORING_TGCRAWDATAMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoring/Monitored.h"

#include "StoreGate/ReadHandleKey.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"

class TgcRawDataMonitorAlgorithm : public AthMonitorAlgorithm {
 public:
  TgcRawDataMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TgcRawDataMonitorAlgorithm();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

 private:
  std::string m_packageName;
  SG::ReadHandleKey<xAOD::MuonContainer> m_MuonContainerKey;
  SG::ReadHandleKey<xAOD::MuonContainer> m_MuonEFContainerKey;
  SG::ReadHandleKey<xAOD::MuonRoIContainer> m_MuonRoIContainerKey;
  std::string m_trigTagList;
  bool m_TagAndProbe;

  std::vector<int> m_Groups_trig2bcid;

  std::vector<int> m_Groups_roiEta2Phi;
  std::vector<int> m_Groups_roiEta;
  std::vector<int> m_Groups_roiPhi_barrel_A;
  std::vector<int> m_Groups_roiPhi_endcap_A;
  std::vector<int> m_Groups_roiPhi_forward_A;
  std::vector<int> m_Groups_roiPhi_barrel_C;
  std::vector<int> m_Groups_roiPhi_endcap_C;
  std::vector<int> m_Groups_roiPhi_forward_C;

  std::vector<int> m_Groups_muEta2Phi;
  std::vector<int> m_Groups_muEta;
  std::vector<int> m_Groups_muPhi_barrel_A;
  std::vector<int> m_Groups_muPhi_endcap_A;
  std::vector<int> m_Groups_muPhi_forward_A;
  std::vector<int> m_Groups_muPhi_barrel_C;
  std::vector<int> m_Groups_muPhi_endcap_C;
  std::vector<int> m_Groups_muPhi_forward_C;
  std::vector<int> m_Groups_muPt_barrel;
  std::vector<int> m_Groups_muPt_endcap;
  std::vector<int> m_Groups_muPt_forward;

  struct TagDef{
    TString eventTrig;
    float threshold;
    bool isolated;
  };
  std::vector<TagDef> m_trigTagDefs;
  struct Muons{
    const xAOD::Muon* muon;
    bool tagged;
    bool isolated;
  };

};
#endif
