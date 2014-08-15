/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPHYSVALMONITORING_MUONVALIDATIONPLOTS_H
#define MUONPHYSVALMONITORING_MUONVALIDATIONPLOTS_H

#include "RecoMuonPlots.h"
#include "TruthMuonPlots.h"
#include "TruthRelatedMuonPlots.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTruth/TruthParticle.h"

class MuonValidationPlots:public PlotBase {
  public:
    MuonValidationPlots(PlotBase* pParent, std::string sDir, std::vector<unsigned int> authors,bool doBinnedResolutionPlots);
    virtual ~MuonValidationPlots();
    void fill(const xAOD::Muon& mu);
    void fill(const xAOD::TruthParticle& truthMu);
    void fill(const xAOD::TruthParticle* truthMu, const xAOD::Muon* mu);
    bool isGoodTruthTrack(const xAOD::TruthParticle& truthMu);

    std::vector<unsigned int> m_selectedAuthors;
    std::vector<std::string> m_authorNames;
    std::vector<std::string> m_truthSelections;    

    std::vector<RecoMuonPlots*> m_oRecoMuonPlots;
    std::vector<TruthMuonPlots*> m_oTruthMuonPlots;
    std::vector<TruthRelatedMuonPlots*> m_oTruthRelatedMuonPlots;

 private:
    void fillRecoMuonPlots(const xAOD::Muon& mu);
    void fillTruthMuonPlots(const xAOD::TruthParticle &truthMu);
};

#endif
