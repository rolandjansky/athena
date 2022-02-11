/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SlowMuonValidationPlots.h"

#include <iostream>

#include "MuonHistUtils/MuonEnumDefs.h"

typedef ElementLink<xAOD::MuonContainer> MuonLink;

SlowMuonValidationPlots::SlowMuonValidationPlots(PlotBase* pParent, const std::string& sDir, bool isData) :
    PlotBase(pParent, sDir), m_isData(isData) {
    // histogram classes for all muons
    m_oSlowMuonRecoPlots = std::make_unique<Muon::SlowMuonParamPlots>(this, "reco/AllMuons/parameters/");
    if (!m_isData) m_oSlowMuonMatchedPlots = std::make_unique< Muon::SlowMuonParamPlots>(this, "matched/AllMuons/parameters/");
}

SlowMuonValidationPlots::~SlowMuonValidationPlots() = default;

void SlowMuonValidationPlots::fill(const xAOD::SlowMuon& smu, const xAOD::Muon& mu, float weight) { fillRecoMuonPlots(smu, mu, weight); }
void SlowMuonValidationPlots::fill(const xAOD::TruthParticle* truthMu, const xAOD::SlowMuon* smu, const xAOD::Muon* mu, float weight) {
    if (smu) fillRecoMuonPlots(*smu, *mu, weight);
    if ((smu) && (truthMu)) { m_oSlowMuonMatchedPlots->fill(*smu, weight); }
}

void SlowMuonValidationPlots::fillRecoMuonPlots(const xAOD::SlowMuon& smu, const xAOD::Muon& /*mu*/, float weight) {
    // fill hists for all muons
    m_oSlowMuonRecoPlots->fill(smu, weight);
}
