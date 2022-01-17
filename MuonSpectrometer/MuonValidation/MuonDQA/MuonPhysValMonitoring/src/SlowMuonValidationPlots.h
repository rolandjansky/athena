/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPHYSVALMONITORING_SLOWMUONVALIDATIONPLOTS_H
#define MUONPHYSVALMONITORING_SLOWMUONVALIDATIONPLOTS_H

#include "MuonHistUtils/SlowMuonParamPlots.h"
#include "xAODMuon/SlowMuon.h"
#include "xAODTruth/TruthParticle.h"

class SlowMuonValidationPlots : public PlotBase {
public:
    SlowMuonValidationPlots(PlotBase* pParent, const std::string& sDir, bool isData);

    virtual ~SlowMuonValidationPlots();
    void fill(const xAOD::SlowMuon& smu, const xAOD::Muon& mu, float weight = 1.0);
    void fill(const xAOD::TruthParticle* truthMu, const xAOD::SlowMuon* smu, const xAOD::Muon* mu, float weight = 1.0);

    Muon::SlowMuonParamPlots* m_oSlowMuonRecoPlots;
    Muon::SlowMuonParamPlots* m_oSlowMuonMatchedPlots;

private:
    void fillRecoMuonPlots(const xAOD::SlowMuon& smu, const xAOD::Muon& mu, float weight = 1.0);
    void fillMatchedMuonPlots(const xAOD::SlowMuon& smu, const xAOD::Muon& mu, float weight = 1.0);

    bool m_isData;
};

#endif
