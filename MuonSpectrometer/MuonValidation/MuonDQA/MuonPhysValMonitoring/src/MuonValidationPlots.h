/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPHYSVALMONITORING_MUONVALIDATIONPLOTS_H
#define MUONPHYSVALMONITORING_MUONVALIDATIONPLOTS_H

#include "MuonHistUtils/MuonTree.h"
#include "MuonHistUtils/RecoMuonPlotOrganizer.h"
#include "MuonHistUtils/TruthMuonPlotOrganizer.h"
#include "MuonHistUtils/TruthRelatedMuonPlotOrganizer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTruth/TruthParticle.h"

class MuonValidationPlots : public PlotBase {
public:
    MuonValidationPlots(PlotBase* pParent, const std::string& sDir, std::vector<int> wps, std::vector<unsigned int> authors, bool isData,
                        bool doBinnedResolutionPlots, bool doSplitSAFMuons, bool doMuonTree = false);

    virtual ~MuonValidationPlots();
    void fill(const xAOD::Muon& mu, float weight = 1.0);
    bool isGoodTruthTrack(const xAOD::TruthParticle& truthMu);
    void fill(const xAOD::TruthParticle& truthMu, float weight = 1.0);
    void fill(const xAOD::TruthParticle* truthMu, const xAOD::Muon* mu, const xAOD::TrackParticleContainer* MSTracks, float weight = 1.0);
    Muon::MuonTree* getMuonTree();
    void fillTreeBranches(const xAOD::Muon& mu);
    void fillTreeBranches(const xAOD::TruthParticle& truthMu);
    void fillTreeBranches(const xAOD::TruthParticle* truthMu, const xAOD::Muon* mu, const xAOD::TrackParticleContainer* MSTracks);
    void fillTree(const xAOD::EventInfo* eventInfo, bool isData);

    std::vector<int> m_selectedWPs;
    std::vector<unsigned int> m_selectedAuthors;
    std::vector<std::string> m_truthSelections;

    Muon::RecoMuonPlotOrganizer* m_oRecoMuonPlots;
    Muon::TruthRelatedMuonPlotOrganizer* m_oTruthRelatedMuonPlots;
    std::vector<Muon::RecoMuonPlotOrganizer*> m_oRecoMuonPlots_perQuality;
    std::vector<Muon::RecoMuonPlotOrganizer*> m_oRecoMuonPlots_perAuthor;
    std::vector<Muon::TruthRelatedMuonPlotOrganizer*> m_oTruthRelatedMuonPlots_perQuality;
    std::vector<Muon::TruthRelatedMuonPlotOrganizer*> m_oTruthRelatedMuonPlots_perAuthor;
    std::vector<Muon::TruthMuonPlotOrganizer*> m_oTruthMuonPlots;

    std::vector<Muon::TruthRelatedMuonPlotOrganizer*> m_oTruthRelatedMuonPlots_SiAssocFwrdMu;
    std::vector<Muon::RecoMuonPlotOrganizer*> m_oRecoMuonPlots_SiAssocFwrdMu;

private:
    void fillRecoMuonPlots(const xAOD::Muon& mu, float weight = 1.0);
    void fillTruthMuonPlots(const xAOD::TruthParticle& truthMu, float weight = 1.0);

    bool m_isData;
    bool m_doSeparateSAFMuons;

    Muon::MuonTree* m_MuonTree;
};

#endif
