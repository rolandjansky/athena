/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DiTauRec/DiTauBuilder.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "DiTauRec/DiTauToolBase.h"
#include "DiTauRec/DiTauCandidateData.h"

#include "xAODTau/DiTauJet.h"
#include "xAODTau/DiTauJetContainer.h"
#include "xAODTau/DiTauJetAuxContainer.h"


DiTauBuilder::DiTauBuilder( const std::string& name, ISvcLocator* pSvcLocator ) : 
    AthAlgorithm( name, pSvcLocator ),
    m_diTauContainerName("DiTauJets"),
    m_diTauAuxContainerName("DiTauAuxJets."),
    m_seedJetName("AntiKt10LCTopoJets"),
    m_minPt(10000),
    m_maxEta(2.5),
    m_Rjet(1.0),
    m_Rsubjet(0.2),
    m_Rcore(0.1),
    m_tools(this)
{
    declareProperty("DiTauContainer", m_diTauContainerName);
    declareProperty("DiTauAuxContainer", m_diTauAuxContainerName);
    declareProperty("SeedJetName", m_seedJetName);
    declareProperty("minPt", m_minPt);
    declareProperty("maxEta", m_maxEta);
    declareProperty("Rjet", m_Rjet);
    declareProperty("Rsubjet", m_Rsubjet);
    declareProperty("Rcore", m_Rcore);
    declareProperty("Tools", m_tools ); 

}


DiTauBuilder::~DiTauBuilder() {}


StatusCode DiTauBuilder::initialize() {
    ATH_MSG_INFO ("Initializing " << name() << "...");

    // no tools allocated
    StatusCode sc;
    if (m_tools.size() == 0) {
        ATH_MSG_ERROR("no tools given!");
        return StatusCode::FAILURE;
    }

    // list allocated tools 
    ToolHandleArray<DiTauToolBase> ::iterator itT = m_tools.begin();
    ToolHandleArray<DiTauToolBase> ::iterator itTE = m_tools.end();
    ATH_MSG_INFO("List of tools in execution sequence:");
    ATH_MSG_INFO("------------------------------------");

    unsigned int tool_count = 0;

    for (; itT != itTE; ++itT) {
        sc = itT->retrieve();
        if (sc.isFailure()) {
            ATH_MSG_WARNING("Cannot find tool named <" << *itT << ">");
        } else {
         ++tool_count;
         ATH_MSG_INFO((*itT)->type() << " - " << (*itT)->name());
     }
    }
    ATH_MSG_INFO(" ");
    ATH_MSG_INFO("------------------------------------");

    if (tool_count == 0) {
        ATH_MSG_ERROR("could not allocate any tool!");
        return StatusCode::FAILURE;
    }


    return StatusCode::SUCCESS;
}


StatusCode DiTauBuilder::finalize() {
    ATH_MSG_INFO ("Finalizing " << name() << "...");

    return StatusCode::SUCCESS;
}


StatusCode DiTauBuilder::execute() {  
    ATH_MSG_DEBUG ("Executing " << name() << "...");
    
    StatusCode sc;

    // ----------------------------------------------------------------------------
    // preparing DiTau Candidate Container and storage in DiTauData
    // ----------------------------------------------------------------------------
    DiTauCandidateData rDiTauData;

    xAOD::DiTauJetContainer* pContainer = 0;
    xAOD::DiTauJetAuxContainer * pAuxContainer = 0;

    pContainer = new xAOD::DiTauJetContainer();
    sc = evtStore()->record(pContainer, m_diTauContainerName);
    if (sc.isFailure()) {
        ATH_MSG_ERROR("could not record DiTauContainer");
        delete pContainer;
        return StatusCode::FAILURE;
    }

    pAuxContainer = new xAOD::DiTauJetAuxContainer();
    sc = evtStore()->record(pAuxContainer, m_diTauAuxContainerName);
    if (sc.isFailure()) {
        ATH_MSG_ERROR("could not record DiTauAuxContainer");
        delete pAuxContainer;
        return StatusCode::FAILURE;
    }

    pContainer->setStore(pAuxContainer);


    // set properties of DiTau Candidate 
    rDiTauData.xAODDiTau = 0;
    rDiTauData.xAODDiTauContainer = pContainer; //
    rDiTauData.diTauAuxContainer = pAuxContainer; //
    rDiTauData.seed = 0;
    rDiTauData.seedContainer = 0;

    rDiTauData.Rjet = m_Rjet;
    rDiTauData.Rsubjet = m_Rsubjet;
    rDiTauData.Rcore = m_Rcore;

    // ----------------------------------------------------------------------------
    // retrieve di-tau seed jets and loop over seeds
    // ----------------------------------------------------------------------------

    const xAOD::JetContainer* pSeedContainer;
    sc = evtStore()->retrieve(pSeedContainer, m_seedJetName);
    if (sc.isFailure() || !pSeedContainer) {
        ATH_MSG_FATAL("could not find seed jets with key:" << m_seedJetName);
        return StatusCode::FAILURE;
    }

    rDiTauData.seedContainer = pSeedContainer;

    for (const auto* seed: *pSeedContainer) {
        ATH_MSG_DEBUG("Seed pt: "<< seed->pt() << "  eta: "<< seed->eta());

        // seed cuts
        if (fabs(seed->pt()) < m_minPt) continue;
        if (fabs(seed->eta()) > m_maxEta) continue;

        // cuts passed
        rDiTauData.seed = seed;

        // create new di-tau candidate
        rDiTauData.xAODDiTau = new xAOD::DiTauJet();
        rDiTauData.xAODDiTauContainer->push_back(rDiTauData.xAODDiTau);

        // handle di-tau candidate
        for (const auto tool: m_tools) {
            sc = tool->execute(&rDiTauData);

            if (sc.isFailure()) break;
        }

        if (sc.isSuccess()) {
            ATH_MSG_DEBUG("all tools executed successfully. Di-Tau candidate registered.");
        }
        else {
            ATH_MSG_DEBUG("seed failed a di-tau criterion. Thrown away.");
            rDiTauData.xAODDiTauContainer->pop_back();
        }
    }

    // // TODO: tool finalizers needed here
    // sc = StatusCode::SUCCESS;

    ATH_MSG_DEBUG("execute tool finializers");
    rDiTauData.xAODDiTau = 0;
    for (const auto tool: m_tools) {
        sc = tool->eventFinalize(&rDiTauData);
        if (!sc.isSuccess())
            return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("end execute()");
    return StatusCode::SUCCESS;
}


