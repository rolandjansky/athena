/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPHYSVALMONITORING_SLOWMUONVALIDATIONPLOTS_H
#define MUONPHYSVALMONITORING_SLOWMUONVALIDATIONPLOTS_H

#include "xAODTruth/TruthParticle.h"
#include "xAODMuon/SlowMuon.h"
#include "MuonHistUtils/SlowMuonParamPlots.h"

class SlowMuonValidationPlots:public PlotBase
{
 public:
  SlowMuonValidationPlots(PlotBase* pParent, std::string sDir, bool isData);

  virtual ~SlowMuonValidationPlots();
  void fill(const xAOD::SlowMuon& smu, const xAOD::Muon& mu);
  void fill(const xAOD::TruthParticle *truthMu, const xAOD::SlowMuon *smu, const xAOD::Muon* mu);

  Muon::SlowMuonParamPlots* m_oSlowMuonRecoPlots;
  Muon::SlowMuonParamPlots* m_oSlowMuonMatchedPlots;

 private:
  void fillRecoMuonPlots(const xAOD::SlowMuon& smu, const xAOD::Muon& mu);
  void fillMatchedMuonPlots(const xAOD::SlowMuon& smu, const xAOD::Muon& mu);

  bool m_isData;

};

#endif
