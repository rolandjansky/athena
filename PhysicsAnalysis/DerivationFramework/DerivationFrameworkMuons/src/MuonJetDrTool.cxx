/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Finds the nearest jet and adds its info to the muon.
#include "DerivationFrameworkMuons/MuonJetDrTool.h"

#include "FourMomUtils/xAODP4Helpers.h"
#include "StoreGate/WriteDecorHandle.h"
// Constructor
DerivationFramework::MuonJetDrTool::MuonJetDrTool(const std::string& t, const std::string& n, const IInterface* p) : AthAlgTool(t, n, p) {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
}
StatusCode DerivationFramework::MuonJetDrTool::initialize() {
    ATH_CHECK(m_muonSGKey.initialize());
    ATH_CHECK(m_jetSGKey.initialize());
    m_jetDR_SGKey = m_muonSGKey.key() + "." + m_jetDR_SGKey.key();
    ATH_CHECK(m_jetDR_SGKey.initialize());
    return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::MuonJetDrTool::addBranches() const {
    const EventContext& ctx = Gaudi::Hive::currentContext();

    // Retrieve main muonicle collection
    SG::ReadHandle<xAOD::MuonContainer> muons{m_muonSGKey, ctx};
    if (!muons.isValid()) {
        ATH_MSG_FATAL("Failed to retrive container " << m_muonSGKey.fullKey());
        return StatusCode::FAILURE;
    }
    SG::ReadHandle<xAOD::JetContainer> jets{m_jetSGKey, ctx};
    if (!jets.isValid()) {
        ATH_MSG_FATAL("Failed to retrieve " << m_jetSGKey.fullKey());
        return StatusCode::FAILURE;
    }
    SG::WriteDecorHandle<xAOD::MuonContainer, float> decorator_jetdR{m_jetDR_SGKey, ctx};

    for (auto muon : *muons) {
        double new_jetdR = FLT_MAX;
        bool found{false};
        for (auto jet : *jets) {
            if (jet->pt() <= m_jetMinPt) continue;
            new_jetdR = std::min(xAOD::P4Helpers::deltaR(jet, muon), new_jetdR);
            found = true;
        }
        decorator_jetdR(*muon) = found ? new_jetdR : -1;
    }
    return StatusCode::SUCCESS;
}
