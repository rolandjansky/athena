/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPHYSVALMONITORING_MUONVALIDATIONPLOTS_H
#define MUONPHYSVALMONITORING_MUONVALIDATIONPLOTS_H

#include "RecoMuonPlots.h"
#include "TruthMuonPlots.h"
#include "TruthRelatedMuonPlots.h"

#include "MuonHistUtils/L1TriggerMuonPlots.h"
#include "MuonHistUtils/EFTriggerMuonPlots.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTrigger/MuonRoI.h"

class MuonValidationPlots:public PlotBase {
  public:
  MuonValidationPlots(PlotBase* pParent, std::string sDir, std::vector<unsigned int> authors, bool isData, bool doBinnedResolutionPlots, bool doTrigMuonL1Validation, bool doTrigMuonL2Validation, bool doTrigMuonEFValidation );

    virtual ~MuonValidationPlots();
    void fill(const xAOD::Muon& mu);
    void fill(const xAOD::MuonRoI &TrigL1mu);
    void fill(const xAOD::Muon& mu, const xAOD::Muon& Trigmu);
    bool isGoodTruthTrack(const xAOD::TruthParticle& truthMu);

    void fill(const xAOD::TruthParticle& truthMu);
    void fill(const xAOD::TruthParticle* truthMu, const xAOD::Muon* mu, const xAOD::TrackParticleContainer* MSTracks);

    std::vector<unsigned int> m_selectedAuthors;
    std::vector<std::string> m_truthSelections;    
    std::vector<std::string> m_TriggerLevels;
 
    RecoMuonPlots *m_oRecoMuonPlots;
    TruthRelatedMuonPlots *m_oTruthRelatedMuonPlots;
    L1TriggerMuonPlots* m_oL1TriggerMuonPlots;    
    std::vector<EFTriggerMuonPlots*> m_oEFTriggerMuonPlots;
    std::vector<RecoMuonPlots*> m_oRecoMuonPlots_perQuality;
    std::vector<RecoMuonPlots*> m_oRecoMuonPlots_perAuthor;
    std::vector<TruthRelatedMuonPlots*> m_oTruthRelatedMuonPlots_perQuality;
    std::vector<TruthRelatedMuonPlots*> m_oTruthRelatedMuonPlots_perAuthor;
    std::vector<TruthMuonPlots*> m_oTruthMuonPlots;    

 private:
    void fillRecoMuonPlots(const xAOD::Muon& mu);
    void fillTruthMuonPlots(const xAOD::TruthParticle &truthMu);
    void fillTriggerMuonPlots(const xAOD::Muon& mu,const xAOD::Muon& Trigmu);
    void fillTriggerMuonPlots(const xAOD::MuonRoI& TrigL1mu);

    bool m_isData;
    bool m_doTrigMuonL1Validation;
    bool m_doTrigMuonL2Validation;
    bool m_doTrigMuonEFValidation;

};

#endif
