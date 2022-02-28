/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerMuonValidationPlots.h"

#include <utility>

#include "MuonHistUtils/MuonEnumDefs.h"
TriggerMuonValidationPlots::TriggerMuonValidationPlots(PlotBase* pParent, const std::string& sDir, std::vector<unsigned int> authors,
                                                       bool /*isData*/, bool doTrigMuonL1Validation, bool doTrigMuonL2Validation,
                                                       bool doTrigMuonEFValidation, std::vector<std::vector<std::string>> ChainSeed,
                                                       std::vector<std::string> L1MuonItems) :
    PlotBase(pParent, sDir),
    m_selectedAuthors(std::move(authors)),
    m_doTrigMuonL1Validation(doTrigMuonL1Validation),
    m_doTrigMuonL2Validation(doTrigMuonL2Validation),
    m_doTrigMuonEFValidation(doTrigMuonEFValidation),
    m_ChainSeed(std::move(ChainSeed)),
    m_L1MuonItems(std::move(L1MuonItems)) {
    for (unsigned int i = 0; i < m_ChainSeed.size(); i++) {
        m_chains.push_back(m_ChainSeed[i][0]);
        m_seeds.push_back(m_ChainSeed[i][1]);
    }

    if (m_doTrigMuonL1Validation) m_oL1TriggerMuonPlots = std::make_unique<L1TriggerMuonPlots>(this, "trigger/L1");
    if (m_doTrigMuonL2Validation) {
        m_oL2TriggerMuonPlots.push_back(std::make_unique<HLTriggerMuonPlots>(this, "trigger/L2/StandAlone"));
        m_oL2TriggerMuonPlots.push_back(std::make_unique<HLTriggerMuonPlots>(this, "trigger/L2/Combined"));
        m_oL2TriggerMuonBarrelResolutionPlots.push_back(
            std::make_unique<ResoTriggerMuonPlots>(this, "trigger/L2/StandAlone/Resolution/BARREL/", ""));
        m_oL2TriggerMuonBarrelResolutionPlots.push_back(std::make_unique<ResoTriggerMuonPlots>(this, "trigger/L2/Combined/Resolution/BARREL/", ""));
        m_oL2TriggerMuonEndcapsResolutionPlots.push_back(
            std::make_unique<ResoTriggerMuonPlots>(this, "trigger/L2/StandAlone/Resolution/ENDCAPS/", ""));
        m_oL2TriggerMuonEndcapsResolutionPlots.push_back(
            std::make_unique<ResoTriggerMuonPlots>(this, "trigger/L2/Combined/Resolution/ENDCAPS/", ""));
        m_oL2TriggerMuonResolutionPlots.push_back(
            std::make_unique<ResoTriggerMuonPlots>(this, "trigger/L2/StandAlone/Resolution/WHOLE_DETECT/", ""));
        m_oL2TriggerMuonResolutionPlots.push_back(std::make_unique<ResoTriggerMuonPlots>(this, "trigger/L2/Combined/Resolution/WHOLE_DETECT/", ""));
    }

    // define a histogram class for each of the selected muon authors
    for (unsigned int i = 0; i < m_selectedAuthors.size(); i++) {
        std::string sAuthor = Muon::EnumDefs::toString((xAOD::Muon::Author)m_selectedAuthors[i]);
        if (m_doTrigMuonEFValidation) m_oEFTriggerMuonPlots.push_back(std::make_unique<HLTriggerMuonPlots>(this, "trigger/EF/" + sAuthor));
        if (m_doTrigMuonEFValidation)
            m_oEFTriggerMuonBarrelResolutionPlots.push_back(
                std::make_unique<ResoTriggerMuonPlots>(this, "trigger/EF/" + sAuthor + "/Resolution/BARREL/", ""));
        if (m_doTrigMuonEFValidation)
            m_oEFTriggerMuonEndcapsResolutionPlots.push_back(
                std::make_unique<ResoTriggerMuonPlots>(this, "trigger/EF/" + sAuthor + "/Resolution/ENDCAPS/", ""));
        if (m_doTrigMuonEFValidation)
            m_oEFTriggerMuonResolutionPlots.push_back(
                std::make_unique<ResoTriggerMuonPlots>(this, "trigger/EF/" + sAuthor + "/Resolution/WHOLE_DETECT/", ""));
    }

    for (unsigned int i = 0; i < m_L1MuonItems.size(); i++) {
        if (m_doTrigMuonEFValidation)
            m_oL1TriggerChainBarrelEfficiencyPlots.push_back(
                std::make_unique<TriggerEfficiencyPlots>(this, "trigger/" + m_L1MuonItems[i] + "_Efficiency/BARREL/"));
        if (m_doTrigMuonEFValidation)
            m_oL1TriggerChainEndcapsEfficiencyPlots.push_back(
                std::make_unique<TriggerEfficiencyPlots>(this, "trigger/" + m_L1MuonItems[i] + "_Efficiency/ENDCAPS/"));
        if (m_doTrigMuonEFValidation)
            m_oL1TriggerChainEfficiencyPlots.push_back(
                std::make_unique<TriggerEfficiencyPlots>(this, "trigger/" + m_L1MuonItems[i] + "_Efficiency/WHOLE_DETECT/"));
    }
    for (unsigned int i = 0; i < m_chains.size(); i++) {
        if (m_doTrigMuonEFValidation)
            m_oEFTriggerChainBarrelEfficiencyPlots.push_back(
                std::make_unique<TriggerEfficiencyPlots>(this, "trigger/" + m_chains[i] + "_Efficiency/BARREL/"));
        if (m_doTrigMuonEFValidation)
            m_oEFTriggerChainEndcapsEfficiencyPlots.push_back(
                std::make_unique<TriggerEfficiencyPlots>(this, "trigger/" + m_chains[i] + "_Efficiency/ENDCAPS/"));
        if (m_doTrigMuonEFValidation)
            m_oEFTriggerChainEfficiencyPlots.push_back(
                std::make_unique<TriggerEfficiencyPlots>(this, "trigger/" + m_chains[i] + "_Efficiency/WHOLE_DETECT/"));
    }
    for (unsigned int i = 0; i < m_chains.size(); i++) {
        if (m_doTrigMuonEFValidation)
            m_oEFTriggerChainBarrelRELEfficiencyPlots.push_back(
                std::make_unique<TriggerEfficiencyPlots>(this, "trigger/" + m_chains[i] + "_wrt_" + m_seeds[i] + "_Efficiency/BARREL/"));
        if (m_doTrigMuonEFValidation)
            m_oEFTriggerChainEndcapsRELEfficiencyPlots.push_back(
                std::make_unique<TriggerEfficiencyPlots>(this, "trigger/" + m_chains[i] + "_wrt_" + m_seeds[i] + "_Efficiency/ENDCAPS/"));
        if (m_doTrigMuonEFValidation)
            m_oEFTriggerChainRELEfficiencyPlots.push_back(
                std::make_unique<TriggerEfficiencyPlots>(this, "trigger/" + m_chains[i] + "_wrt_" + m_seeds[i] + "_Efficiency/WHOLE_DETECT/"));
    }
    PlateauTreshold = 0.;
}

TriggerMuonValidationPlots::~TriggerMuonValidationPlots() = default;

void TriggerMuonValidationPlots::fillTriggerMuonPlots(const xAOD::MuonRoI& TrigL1mu) { m_oL1TriggerMuonPlots->fill(TrigL1mu); }

void TriggerMuonValidationPlots::fillTriggerMuonPlots(const xAOD::L2StandAloneMuon& L2SAmu) { m_oL2TriggerMuonPlots[0]->fill(L2SAmu); }

void TriggerMuonValidationPlots::fillTriggerMuonPlots(const xAOD::L2CombinedMuon& L2CBmu) { m_oL2TriggerMuonPlots[1]->fill(L2CBmu); }

void TriggerMuonValidationPlots::fillTriggerMuonPlots(const xAOD::Muon& Trigmu) {
    for (unsigned int i = 0; i < m_selectedAuthors.size(); i++) {
        if (Trigmu.isAuthor((xAOD::Muon::Author)m_selectedAuthors[i])) {
            // if (Trigmu.isAuthor( (xAOD::Muon::Author)m_selectedAuthors[i] ) || m_selectedAuthors[i]==xAOD::Muon::NumberOfMuonAuthors) {
            m_oEFTriggerMuonPlots[i]->fill(Trigmu);
        }
    }
}

void TriggerMuonValidationPlots::fillTriggerMuonPlots(const xAOD::Muon& Trigmu, const xAOD::Muon& Recomu) {
    for (unsigned int i = 0; i < m_selectedAuthors.size(); i++) {
        if (Trigmu.isAuthor((xAOD::Muon::Author)m_selectedAuthors[i])) {
            // if (Trigmu.isAuthor( (xAOD::Muon::Author)m_selectedAuthors[i] ) || m_selectedAuthors[i]==xAOD::Muon::NumberOfMuonAuthors) {
            m_oEFTriggerMuonResolutionPlots[i]->fill(Trigmu, Recomu);
            if (std::abs(Recomu.eta()) < 1.05) m_oEFTriggerMuonBarrelResolutionPlots[i]->fill(Trigmu, Recomu);
            if (std::abs(Recomu.eta()) > 1.05) m_oEFTriggerMuonEndcapsResolutionPlots[i]->fill(Trigmu, Recomu);
        }
    }
}

void TriggerMuonValidationPlots::fillTriggerMuonPlots(const xAOD::L2StandAloneMuon& L2SAmu, const xAOD::Muon& Recomu) {
    m_oL2TriggerMuonResolutionPlots[0]->fill(L2SAmu, Recomu);
    if (std::abs(Recomu.eta()) < 1.05) m_oL2TriggerMuonBarrelResolutionPlots[0]->fill(L2SAmu, Recomu);
    if (std::abs(Recomu.eta()) > 1.05) m_oL2TriggerMuonEndcapsResolutionPlots[0]->fill(L2SAmu, Recomu);
}

void TriggerMuonValidationPlots::fillTriggerMuonPlots(const xAOD::L2CombinedMuon& L2CBmu, const xAOD::Muon& Recomu) {
    m_oL2TriggerMuonResolutionPlots[1]->fill(L2CBmu, Recomu);
    if (std::abs(Recomu.eta()) < 1.05) m_oL2TriggerMuonBarrelResolutionPlots[1]->fill(L2CBmu, Recomu);
    if (std::abs(Recomu.eta()) > 1.05) m_oL2TriggerMuonEndcapsResolutionPlots[1]->fill(L2CBmu, Recomu);
}

void TriggerMuonValidationPlots::fill(const xAOD::Muon& Trigmu) { fillTriggerMuonPlots(Trigmu); }

void TriggerMuonValidationPlots::fill(const xAOD::Muon& Trigmu, const xAOD::Muon& Recomu) { fillTriggerMuonPlots(Trigmu, Recomu); }

void TriggerMuonValidationPlots::fill(const xAOD::L2StandAloneMuon& L2SAmu, const xAOD::Muon& Recomu) {
    fillTriggerMuonPlots(L2SAmu, Recomu);
}

void TriggerMuonValidationPlots::fill(const xAOD::L2CombinedMuon& L2CBmu, const xAOD::Muon& Recomu) {
    fillTriggerMuonPlots(L2CBmu, Recomu);
}

void TriggerMuonValidationPlots::fill(const xAOD::MuonRoI& TrigL1mu) { fillTriggerMuonPlots(TrigL1mu); }

void TriggerMuonValidationPlots::fill(const xAOD::L2StandAloneMuon& L2SAmu) { fillTriggerMuonPlots(L2SAmu); }

void TriggerMuonValidationPlots::fill(const xAOD::L2CombinedMuon& L2CBmu) { fillTriggerMuonPlots(L2CBmu); }

void TriggerMuonValidationPlots::fillNumEff(const xAOD::Muon& mu, const std::string& selectedChain) {
    float PlateauTreshold = findTrigTreshold(selectedChain);
    PlateauTreshold = PlateauTreshold * 1000. * 1.05;
    for (unsigned int i = 0; i < m_chains.size(); i++) {
        if (m_chains[i] == selectedChain) {
            m_oEFTriggerChainEfficiencyPlots[i]->fillNumerator(mu, PlateauTreshold);
            if (((mu.eta()) > -1.05) && ((mu.eta()) < 1.05)) m_oEFTriggerChainBarrelEfficiencyPlots[i]->fillNumerator(mu, PlateauTreshold);
            if (((mu.eta()) < -1.05) || ((mu.eta()) > 1.05)) m_oEFTriggerChainEndcapsEfficiencyPlots[i]->fillNumerator(mu, PlateauTreshold);
        }
    }
}

void TriggerMuonValidationPlots::fillDenEff(const xAOD::Muon& mu, const std::string& selectedChain) {
    float PlateauTreshold = findTrigTreshold(selectedChain);
    PlateauTreshold = PlateauTreshold * 1000. * 1.05;
    for (unsigned int i = 0; i < m_chains.size(); i++) {
        if (m_chains[i] == selectedChain) {
            m_oEFTriggerChainEfficiencyPlots[i]->fillDenominator(mu, PlateauTreshold);
            if (((mu.eta()) > -1.05) && ((mu.eta()) < 1.05))
                m_oEFTriggerChainBarrelEfficiencyPlots[i]->fillDenominator(mu, PlateauTreshold);
            if (((mu.eta()) < -1.05) || ((mu.eta()) > 1.05))
                m_oEFTriggerChainEndcapsEfficiencyPlots[i]->fillDenominator(mu, PlateauTreshold);
        }
    }
}

void TriggerMuonValidationPlots::fillFeatPlots(const xAOD::Muon& mu, const std::string& selectedChain) {
    float PlateauTreshold = findTrigTreshold(selectedChain);
    PlateauTreshold = PlateauTreshold * 1000. * 1.05;
    for (unsigned int i = 0; i < m_chains.size(); i++) {
        if (m_chains[i] == selectedChain) {
            m_oEFTriggerChainEfficiencyPlots[i]->fillFeatures(mu, PlateauTreshold);
            if (((mu.eta()) > -1.05) && ((mu.eta()) < 1.05)) m_oEFTriggerChainBarrelEfficiencyPlots[i]->fillFeatures(mu, PlateauTreshold);
            if (((mu.eta()) < -1.05) || ((mu.eta()) > 1.05)) m_oEFTriggerChainEndcapsEfficiencyPlots[i]->fillFeatures(mu, PlateauTreshold);
        }
    }
}

void TriggerMuonValidationPlots::fillNumL1Eff(const xAOD::Muon& mu, const std::string& selectedL1MuonItem) {
    float PlateauTreshold = findTrigTreshold(selectedL1MuonItem);
    PlateauTreshold = PlateauTreshold * 1000. * 1.05;
    for (unsigned int i = 0; i < m_L1MuonItems.size(); i++) {
        if (m_L1MuonItems[i] == selectedL1MuonItem) {
            m_oL1TriggerChainEfficiencyPlots[i]->fillNumerator(mu, PlateauTreshold);
            if (((mu.eta()) > -1.05) && ((mu.eta()) < 1.05)) m_oL1TriggerChainBarrelEfficiencyPlots[i]->fillNumerator(mu, PlateauTreshold);
            if (((mu.eta()) < -1.05) || ((mu.eta()) > 1.05)) m_oL1TriggerChainEndcapsEfficiencyPlots[i]->fillNumerator(mu, PlateauTreshold);
        }
    }
}

void TriggerMuonValidationPlots::fillDenL1Eff(const xAOD::Muon& mu, const std::string& selectedL1MuonItem) {
    float PlateauTreshold = findTrigTreshold(selectedL1MuonItem);
    PlateauTreshold = PlateauTreshold * 1000. * 1.05;
    for (unsigned int i = 0; i < m_L1MuonItems.size(); i++) {
        if (m_L1MuonItems[i] == selectedL1MuonItem) {
            m_oL1TriggerChainEfficiencyPlots[i]->fillDenominator(mu, PlateauTreshold);
            if (((mu.eta()) > -1.05) && ((mu.eta()) < 1.05))
                m_oL1TriggerChainBarrelEfficiencyPlots[i]->fillDenominator(mu, PlateauTreshold);
            if (((mu.eta()) < -1.05) || ((mu.eta()) > 1.05))
                m_oL1TriggerChainEndcapsEfficiencyPlots[i]->fillDenominator(mu, PlateauTreshold);
        }
    }
}
void TriggerMuonValidationPlots::fillFeatPlots(const xAOD::MuonRoI& TrigL1mu, const std::string& selectedL1MuonItem) {
    float PlateauTreshold = findTrigTreshold(selectedL1MuonItem);
    PlateauTreshold = PlateauTreshold * 1000. * 1.05;
    xAOD::MuonContainer* ROIinMuons = new xAOD::MuonContainer;
    xAOD::MuonAuxContainer* ROIinMuonsAux = new xAOD::MuonAuxContainer;
    ROIinMuons->setStore(ROIinMuonsAux);
    xAOD::Muon* myROI = new xAOD::Muon();
    ROIinMuons->push_back(myROI);
    myROI->setP4(TrigL1mu.thrValue(), TrigL1mu.eta(), TrigL1mu.phi());

    for (unsigned int i = 0; i < m_L1MuonItems.size(); i++) {
        if (m_L1MuonItems[i] == selectedL1MuonItem) {
            m_oL1TriggerChainEfficiencyPlots[i]->fillFeatures(*myROI, PlateauTreshold);
            if (((TrigL1mu.eta()) > -1.05) && ((TrigL1mu.eta()) < 1.05))
                m_oL1TriggerChainBarrelEfficiencyPlots[i]->fillFeatures(*myROI, PlateauTreshold);
            if (((TrigL1mu.eta()) < -1.05) || ((TrigL1mu.eta()) > 1.05))
                m_oL1TriggerChainEndcapsEfficiencyPlots[i]->fillFeatures(*myROI, PlateauTreshold);
        }
    }
    delete ROIinMuons;
    delete ROIinMuonsAux;
}

void TriggerMuonValidationPlots::fillDenRELEff(const xAOD::Muon& mu, const std::string& selectedChain) {
    float PlateauTreshold = findTrigTreshold(selectedChain);
    PlateauTreshold = PlateauTreshold * 1000. * 1.05;
    for (unsigned int i = 0; i < m_chains.size(); i++) {
        if (m_chains[i] == selectedChain) {
            m_oEFTriggerChainRELEfficiencyPlots[i]->fillDenominator(mu, PlateauTreshold);
            if (((mu.eta()) > -1.05) && ((mu.eta()) < 1.05))
                m_oEFTriggerChainBarrelRELEfficiencyPlots[i]->fillDenominator(mu, PlateauTreshold);
            if (((mu.eta()) < -1.05) || ((mu.eta()) > 1.05))
                m_oEFTriggerChainEndcapsRELEfficiencyPlots[i]->fillDenominator(mu, PlateauTreshold);
        }
    }
}

void TriggerMuonValidationPlots::fillNumRELEff(const xAOD::Muon& mu, const std::string& selectedChain) {
    float PlateauTreshold = findTrigTreshold(selectedChain);
    PlateauTreshold = PlateauTreshold * 1000. * 1.05;
    for (unsigned int i = 0; i < m_chains.size(); i++) {
        if (m_chains[i] == selectedChain) {
            m_oEFTriggerChainRELEfficiencyPlots[i]->fillNumerator(mu, PlateauTreshold);
            if (((mu.eta()) > -1.05) && ((mu.eta()) < 1.05))
                m_oEFTriggerChainBarrelRELEfficiencyPlots[i]->fillNumerator(mu, PlateauTreshold);
            if (((mu.eta()) < -1.05) || ((mu.eta()) > 1.05))
                m_oEFTriggerChainEndcapsRELEfficiencyPlots[i]->fillNumerator(mu, PlateauTreshold);
        }
    }
}

float TriggerMuonValidationPlots::findTrigTreshold(const TString& chain) {
    float PlateauTreshold;
    std::string str(chain);
    TString temp;
    if (str.find("HLT_") != std::string::npos) {
        for (unsigned int l = 0; l < str.size(); l++) {
            if (isdigit(str[l])) {
                for (unsigned int a = l; a < str.size(); a++) {
                    // if (str[a]=='_') break;
                    if ((isdigit(str[a])) == 0) break;
                    temp += str[a];
                }
                break;
            }
        }
    }
    if (str.find("L1_MU") != std::string::npos) {
        for (unsigned int l = 0; l < str.size(); l++) {
            if ((str[l]) == 'U') {
                for (unsigned int a = l + 1; a < str.size(); a++) {
                    // if (str[a]=='_') break;
                    temp += str[a];
                }
                break;
            }
        }
    }
    PlateauTreshold = temp.Atof();
    return PlateauTreshold;
}
