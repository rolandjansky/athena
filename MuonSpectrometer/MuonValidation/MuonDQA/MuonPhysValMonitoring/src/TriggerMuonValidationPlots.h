/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPHYSVALMONITORING_TRIGGERMUONVALIDATIONPLOTS_H
#define MUONPHYSVALMONITORING_TRIGGERMUONVALIDATIONPLOTS_H

#include "MuonHistUtils/HLTriggerMuonPlots.h"
#include "MuonHistUtils/L1TriggerMuonPlots.h"
#include "ResoTriggerMuonPlots.h"
#include "TriggerEfficiencyPlots.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuon.h"
#include "xAODTrigMuon/L2StandAloneMuon.h"
#include "xAODTrigger/MuonRoI.h"

class TriggerMuonValidationPlots : public PlotBase {
public:
    TriggerMuonValidationPlots(PlotBase* pParent, const std::string& sDir, std::vector<unsigned int> authors, bool isData,
                               bool doTrigMuonL1Validation, bool doTrigMuonL2Validation, bool doTrigMuonEFValidation,
                               std::vector<std::vector<std::string>> ChainSeed, std::vector<std::string> L1MuonItems);

    float PlateauTreshold;
    virtual ~TriggerMuonValidationPlots();
    void fill(const xAOD::MuonRoI& TrigL1mu);
    void fill(const xAOD::L2StandAloneMuon& L2SAmu);
    void fill(const xAOD::L2CombinedMuon& L2CBmu);
    void fill(const xAOD::Muon& Trigmu);
    void fill(const xAOD::Muon& Trigmu, const xAOD::Muon& Recomu);
    void fill(const xAOD::L2StandAloneMuon& L2SAmu, const xAOD::Muon& Recomu);
    void fill(const xAOD::L2CombinedMuon& L2CBmu, const xAOD::Muon& Recomu);
    void fillNumEff(const xAOD::Muon& Trigmu, const std::string& selectedChain);
    void fillDenEff(const xAOD::Muon& Trigmu, const std::string& selectedChain);
    void fillNumL1Eff(const xAOD::Muon& Trigmu, const std::string& selectedChain);
    void fillDenL1Eff(const xAOD::Muon& Trigmu, const std::string& selectedChain);
    void fillFeatPlots(const xAOD::Muon& Trigmu, const std::string& selectedChain);
    void fillFeatPlots(const xAOD::MuonRoI& TrigL1mu, const std::string& selectedChain);
    void fillDenRELEff(const xAOD::Muon& Trigmu, const std::string& selectedChain);
    void fillNumRELEff(const xAOD::Muon& Trigmu, const std::string& selectedChain);

    std::vector<unsigned int> m_selectedAuthors;
    std::vector<std::string> m_chains;
    std::vector<std::string> m_seeds;
    std::unique_ptr<L1TriggerMuonPlots> m_oL1TriggerMuonPlots{};
    std::vector<std::unique_ptr<HLTriggerMuonPlots>> m_oEFTriggerMuonPlots;
    std::vector<std::unique_ptr<HLTriggerMuonPlots>> m_oL2TriggerMuonPlots;
    std::vector<std::unique_ptr<ResoTriggerMuonPlots>> m_oEFTriggerMuonBarrelResolutionPlots;
    std::vector<std::unique_ptr<ResoTriggerMuonPlots>> m_oEFTriggerMuonEndcapsResolutionPlots;
    std::vector<std::unique_ptr<ResoTriggerMuonPlots>> m_oEFTriggerMuonResolutionPlots;
    std::vector<std::unique_ptr<ResoTriggerMuonPlots>> m_oL2TriggerMuonBarrelResolutionPlots;
    std::vector<std::unique_ptr<ResoTriggerMuonPlots>> m_oL2TriggerMuonEndcapsResolutionPlots;
    std::vector<std::unique_ptr<ResoTriggerMuonPlots>> m_oL2TriggerMuonResolutionPlots;
    std::vector<std::unique_ptr<TriggerEfficiencyPlots>> m_oL1TriggerChainEfficiencyPlots;
    std::vector<std::unique_ptr<TriggerEfficiencyPlots>> m_oL1TriggerChainBarrelEfficiencyPlots;
    std::vector<std::unique_ptr<TriggerEfficiencyPlots>> m_oL1TriggerChainEndcapsEfficiencyPlots;
    std::vector<std::unique_ptr<TriggerEfficiencyPlots>> m_oEFTriggerChainEfficiencyPlots;
    std::vector<std::unique_ptr<TriggerEfficiencyPlots>> m_oEFTriggerChainBarrelEfficiencyPlots;
    std::vector<std::unique_ptr<TriggerEfficiencyPlots>> m_oEFTriggerChainEndcapsEfficiencyPlots;
    std::vector<std::unique_ptr<TriggerEfficiencyPlots>> m_oEFTriggerChainRELEfficiencyPlots;
    std::vector<std::unique_ptr<TriggerEfficiencyPlots>> m_oEFTriggerChainBarrelRELEfficiencyPlots;
    std::vector<std::unique_ptr<TriggerEfficiencyPlots>> m_oEFTriggerChainEndcapsRELEfficiencyPlots;

private:
    void fillTriggerMuonPlots(const xAOD::Muon& Trigmu);
    void fillTriggerMuonPlots(const xAOD::Muon& Trigmu, const xAOD::Muon& Recomu);
    void fillTriggerMuonPlots(const xAOD::L2StandAloneMuon& L2SAmu, const xAOD::Muon& Recomu);
    void fillTriggerMuonPlots(const xAOD::L2CombinedMuon& L2CBmu, const xAOD::Muon& Recomu);
    void fillTriggerMuonPlots(const xAOD::MuonRoI& TrigL1mu);
    void fillTriggerMuonPlots(const xAOD::L2StandAloneMuon& L2SAmu);
    void fillTriggerMuonPlots(const xAOD::L2CombinedMuon& L2CBmu);
    float findTrigTreshold(const TString& chain);

    bool m_doTrigMuonL1Validation;
    bool m_doTrigMuonL2Validation;
    bool m_doTrigMuonEFValidation;
    std::vector<std::vector<std::string>> m_ChainSeed;
    std::vector<std::string> m_L1MuonItems;
};

#endif
