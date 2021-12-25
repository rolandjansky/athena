/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPHYSVALMONITORING_RESOTRIGGERMUONPLOTS_H
#define MUONPHYSVALMONITORING_RESOTRIGGERMUONPLOTS_H


#include "TrkValHistUtils/PlotBase.h"
#include "xAODMuon/Muon.h"
#include "xAODTrigMuon/L2StandAloneMuon.h"
#include "xAODTrigMuon/L2CombinedMuon.h"

namespace Muon{
  
class ResoTriggerMuonPlots:public PlotBase {
 public:
  ResoTriggerMuonPlots(PlotBase* pParent, const std::string& sDir, std::string sType="");
  void fill(const xAOD::Muon& Trigmu, const xAOD::Muon& Recomu);
  void fill(const xAOD::L2StandAloneMuon& L2SAmu, const xAOD::Muon& Recomu);
  void fill(const xAOD::L2CombinedMuon& L2CBmu, const xAOD::Muon& Recomu);

  TH1* Res_pT;
  TH1* Res_eta;
  TH1* Res_phi;
  TH2* Res_pT_vs_pT;
  TH2* Res_eta_vs_pT;
  TH2* Res_phi_vs_pT;
  std::vector<TH2*> Res_pT_vs_eta;
  std::vector<TH2*> Res_pT_vs_phi;
  std::vector<TH2*> Res_eta_vs_eta;
  std::vector<TH2*> Res_phi_vs_phi;

  
 private:
  std::string m_sType;
  std::vector<std::string> m_pt_slices;
  std::vector<double> m_etaBins;
};


}

#endif
