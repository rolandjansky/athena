/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPHYSVALMONITORING_TRIGGERMUONVALIDATIONPLOTS_H
#define MUONPHYSVALMONITORING_TRIGGERMUONVALIDATIONPLOTS_H

#include "RecoRelatedTriggerMuonPlots.h"

#include "MuonHistUtils/L1TriggerMuonPlots.h"
#include "MuonHistUtils/HLTriggerMuonPlots.h"
#include "xAODTrigMuon/L2StandAloneMuon.h"
#include "xAODTrigMuon/L2CombinedMuon.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTrigger/MuonRoI.h"

class TriggerMuonValidationPlots:public PlotBase {
  public:
  TriggerMuonValidationPlots(PlotBase* pParent, std::string sDir, std::vector<unsigned int> authors, bool isData, bool doBinnedResolutionPlots, bool doTrigMuonL1Validation, bool doTrigMuonL2Validation, bool doTrigMuonEFValidation);

    virtual ~TriggerMuonValidationPlots();
    void fill(const xAOD::MuonRoI &TrigL1mu);
    void fill(const xAOD::L2StandAloneMuon &L2SAmu);
    void fill(const xAOD::L2CombinedMuon &L2CBmu);
    void fill(const xAOD::Muon& Trigmu);
    void fill(const xAOD::Muon& Trigmu, const xAOD::Muon& Recomu);

    std::vector<unsigned int> m_selectedAuthors;

    L1TriggerMuonPlots* m_oL1TriggerMuonPlots;    
    std::vector<HLTriggerMuonPlots*> m_oEFTriggerMuonPlots;
    std::vector<HLTriggerMuonPlots*> m_oL2TriggerMuonPlots;
    RecoRelatedTriggerMuonPlots* m_oL1TriggerMuonResolutionPlots;
    std::vector<RecoRelatedTriggerMuonPlots*> m_oL2TriggerMuonResolutionPlots;
    std::vector<RecoRelatedTriggerMuonPlots*> m_oEFTriggerMuonResolutionPlots;

 private:
    void fillTriggerMuonPlots(const xAOD::Muon& Trigmu);
    void fillTriggerMuonPlots(const xAOD::Muon& Trigmu, const xAOD::Muon& Recomu);
    void fillTriggerMuonPlots(const xAOD::MuonRoI& TrigL1mu);
    void fillTriggerMuonPlots(const xAOD::L2StandAloneMuon& L2SAmu);
    void fillTriggerMuonPlots(const xAOD::L2CombinedMuon& L2CBmu);

    bool m_isData;
    bool m_doTrigMuonL1Validation;
    bool m_doTrigMuonL2Validation;
    bool m_doTrigMuonEFValidation;

};

#endif
