/*
    Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TauAODRunnerAlg.h"

TauAODRunnerAlg::TauAODRunnerAlg(const std::string &name, ISvcLocator *pSvcLocator) : 
    AthAlgorithm(name, pSvcLocator) {}

StatusCode TauAODRunnerAlg::initialize() {
    if (m_modificationTools.empty()) {
        ATH_MSG_ERROR("no mod tools given!");
        return StatusCode::FAILURE;
    }
    ATH_CHECK(m_tauContainer.initialize());
    ATH_CHECK(m_pi0ClusterInputContainer.initialize());
    ATH_CHECK(m_tauOutContainer.initialize());
    ATH_CHECK(m_pi0Container.initialize());
    ATH_CHECK(m_neutralPFOOutputContainer.initialize());
    ATH_CHECK(m_chargedPFOOutputContainer.initialize());
    ATH_CHECK(m_hadronicPFOOutputContainer.initialize());
    ATH_CHECK(m_tauTrackOutputContainer.initialize());
    ATH_CHECK(m_vertexOutputContainer.initialize());

    ATH_CHECK(m_modificationTools.retrieve());
    ATH_CHECK(m_officialTools.retrieve());

    ATH_MSG_INFO("List of modification tools in execution sequence:");
    ATH_MSG_INFO("------------------------------------");
    uint tool_count = 0;
    for (const auto &tool : m_modificationTools) {
        ++tool_count;
        ATH_MSG_INFO(tool->type() << " - " << tool->name());
    }
    ATH_MSG_INFO("List of official tools in execution sequence:");
    ATH_MSG_INFO("------------------------------------");
    for (const auto &tool : m_officialTools) {
        ++tool_count;
        ATH_MSG_INFO(tool->type() << " - " << tool->name());
    }
    ATH_MSG_INFO("------------------------------------");
    if (tool_count == 0) {
        ATH_MSG_ERROR("could not allocate any tool!");
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}

StatusCode TauAODRunnerAlg::execute() {
    // Read in tau jets
    SG::ReadHandle<xAOD::TauJetContainer> tauInputHandle(m_tauContainer);
    if (!tauInputHandle.isValid()) {
        ATH_MSG_ERROR("Could not retrieve HiveDataObj with key " << tauInputHandle.key());
        return StatusCode::FAILURE;
    }
    const xAOD::TauJetContainer *pTauContainer = tauInputHandle.cptr();
    SG::WriteHandle<xAOD::TauTrackContainer> outputTauTrackHandle(m_tauTrackOutputContainer);
    ATH_CHECK(outputTauTrackHandle.record(std::make_unique<xAOD::TauTrackContainer>(), std::make_unique<xAOD::TauTrackAuxContainer>()));
    xAOD::TauTrackContainer *newTauTrkCon = outputTauTrackHandle.ptr();
    SG::WriteHandle<xAOD::TauJetContainer> outputTauHandle(m_tauOutContainer);
    ATH_CHECK(outputTauHandle.record(std::make_unique<xAOD::TauJetContainer>(), std::make_unique<xAOD::TauJetAuxContainer>()));
    xAOD::TauJetContainer *newTauCon = outputTauHandle.ptr();

    static const SG::AuxElement::Accessor<ElementLink<xAOD::TauJetContainer>> acc_ori_tau_link("originalTauJet");
    for (const xAOD::TauJet *tau : *pTauContainer) {
        // deep copy the tau container
        xAOD::TauJet *newTau = new xAOD::TauJet();
        newTauCon->push_back(newTau);
        *newTau = *tau;
        //link the original tau to the deepcopy
        ElementLink<xAOD::TauJetContainer> link_to_ori_tau;
        link_to_ori_tau.toContainedElement(*pTauContainer, tau);
        acc_ori_tau_link(*newTau) = link_to_ori_tau;
        //clear the tautrack links to allow relinking.
        newTau->clearTauTrackLinks();
        for (auto tauTrk : tau->allTracks()) {
            xAOD::TauTrack *newTauTrk = new xAOD::TauTrack();
            // deep copy the tau track
            newTauTrkCon->push_back(newTauTrk);
            *newTauTrk = *tauTrk;
            ElementLink<xAOD::TauTrackContainer> linkToTauTrack;
            // relink the tautrack
            linkToTauTrack.toContainedElement(*newTauTrkCon, newTauTrk);
            newTau->addTauTrackLink(linkToTauTrack);
        }
    }
    // Read the CaloClusterContainer
    SG::ReadHandle<xAOD::CaloClusterContainer> pi0ClusterInHandle( m_pi0ClusterInputContainer );
    if (!pi0ClusterInHandle.isValid()) {
        ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << pi0ClusterInHandle.key());
        return StatusCode::FAILURE;
    }
    const xAOD::CaloClusterContainer * pi0ClusterContainer = pi0ClusterInHandle.cptr();
    // write charged PFO container
    SG::WriteHandle<xAOD::PFOContainer> chargedPFOHandle( m_chargedPFOOutputContainer );
    ATH_CHECK(chargedPFOHandle.record(std::make_unique<xAOD::PFOContainer>(), std::make_unique<xAOD::PFOAuxContainer>()));
    xAOD::PFOContainer* chargedPFOContainer = chargedPFOHandle.ptr();
    // write neutral PFO container
    SG::WriteHandle<xAOD::PFOContainer> neutralPFOHandle( m_neutralPFOOutputContainer );
    ATH_CHECK(neutralPFOHandle.record(std::make_unique<xAOD::PFOContainer>(), std::make_unique<xAOD::PFOAuxContainer>()));
    xAOD::PFOContainer* neutralPFOContainer = neutralPFOHandle.ptr();
    // write pi0 container
    SG::WriteHandle<xAOD::ParticleContainer> pi0Handle(m_pi0Container);
    ATH_CHECK(pi0Handle.record(std::make_unique<xAOD::ParticleContainer>(), std::make_unique<xAOD::ParticleAuxContainer>()));
    xAOD::ParticleContainer *pi0Container = pi0Handle.ptr();
    // write hadronic cluster PFO container
    SG::WriteHandle<xAOD::PFOContainer> hadronicPFOHandle( m_hadronicPFOOutputContainer );
    ATH_CHECK(hadronicPFOHandle.record(std::make_unique<xAOD::PFOContainer>(), std::make_unique<xAOD::PFOAuxContainer>()));
    xAOD::PFOContainer* hadronicClusterPFOContainer = hadronicPFOHandle.ptr();
    // write secondary vertices
    SG::WriteHandle<xAOD::VertexContainer> vertOutHandle( m_vertexOutputContainer );
    ATH_CHECK(vertOutHandle.record(std::make_unique<xAOD::VertexContainer>(), std::make_unique<xAOD::VertexAuxContainer>()));
    xAOD::VertexContainer* pSecVtxContainer = vertOutHandle.ptr();
    int n_tau_modified = 0;
    static const SG::AuxElement::Accessor<char> acc_modified("ModifiedInAOD");
    for (xAOD::TauJet *pTau : *newTauCon) {
        // Loop stops when Failure indicated by one of the tools
        StatusCode sc;
        //add a identifier of if the tau is modifed by the mod tools
        acc_modified(*pTau) = static_cast<char>(false);
        // iterate over the copy
        for (ToolHandle<ITauToolBase> &tool : m_modificationTools) {
            ATH_MSG_DEBUG("RunnerAlg Invoking tool " << tool->name());
            sc = tool->execute(*pTau);
            if (sc.isFailure()) break;
        }
        if (sc.isSuccess()) ATH_MSG_VERBOSE("The tau candidate has been modified successfully by the invoked modification tools.");
        // if tau is not modified by the above tools, never mind running the tools afterward
        if (static_cast<bool>(isTauModified(pTau))) {
            n_tau_modified++;
            for (ToolHandle<ITauToolBase> &tool : m_officialTools) {
                ATH_MSG_DEBUG("RunnerAlg Invoking tool " << tool->name());
                if (tool->type() == "TauPi0ClusterCreator")
                    sc = tool->executePi0ClusterCreator(*pTau, *neutralPFOContainer, *hadronicClusterPFOContainer, *pi0ClusterContainer);
                else if (tool->type() == "TauVertexVariables")
                    sc = tool->executeVertexVariables(*pTau, *pSecVtxContainer);
                else if (tool->type() == "TauPi0ClusterScaler")
                    sc = tool->executePi0ClusterScaler(*pTau, *neutralPFOContainer, *chargedPFOContainer);
                else if (tool->type() == "TauPi0ScoreCalculator")
                    sc = tool->executePi0nPFO(*pTau, *neutralPFOContainer);
                else if (tool->type() == "TauPi0Selector")
                    sc = tool->executePi0nPFO(*pTau, *neutralPFOContainer);
                else if (tool->type() == "PanTau::PanTauProcessor")
                    sc = tool->executePanTau(*pTau, *pi0Container);
                else if (tool->type() == "tauRecTools::TauTrackRNNClassifier")
                    sc = tool->executeTrackClassifier(*pTau, *newTauTrkCon);
                else
                    sc = tool->execute(*pTau);
                if (sc.isFailure()) break;
            }
            if (sc.isSuccess()) ATH_MSG_VERBOSE("The tau candidate has been modified successfully by the invoked official tools.");
        }
    } 
    ATH_MSG_VERBOSE("The tau candidate container has been modified by the rest of the tools");
    ATH_MSG_DEBUG(n_tau_modified << " / " << pTauContainer->size() <<" taus were modified");
    return StatusCode::SUCCESS;
}

//helper 
bool TauAODRunnerAlg::isTauModified(const xAOD::TauJet* newtau) const {
    static const SG::AuxElement::ConstAccessor<char> acc_modified("ModifiedInAOD");
    return acc_modified(*newtau);
}
