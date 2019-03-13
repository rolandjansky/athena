/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEFCaloIsolation.h"
#include "TrigConfHLTData/HLTTriggerElement.h"

#include "xAODMuon/MuonContainer.h"
#include "RecoToolInterfaces/ICaloCellIsolationTool.h"
#include "RecoToolInterfaces/ICaloTopoClusterIsolationTool.h"
#include "RecoToolInterfaces/IsolationCommon.h"

#include "TLorentzVector.h"
#include <iostream>
#include <deque>

ATLAS_NO_CHECK_FILE_THREAD_SAFETY;  // legacy trigger code

// ------------------------------------------------------------------------------------------------------
/**
 * Standard constructor for the calo isolation algo.
 */
TrigMuonEFCaloIsolation::TrigMuonEFCaloIsolation(const std::string &name, ISvcLocator *pSvcLocator) :
    FexAlgo(name, pSvcLocator),
    m_requireCombined(false),
    m_debug(false),
    m_caloTopoClusterIsolationTool("xAOD__CaloIsolationTool"),
    m_etiso_cone2(),
    m_etiso_cone3(),
    m_etiso_cone4() {
    declareProperty("RequireCombinedMuon",          m_requireCombined);
    declareProperty("CaloTopoClusterIsolationTool", m_caloTopoClusterIsolationTool);
    declareProperty("HistoPathBase",                m_histo_path_base = "/EXPERT/");
    declareProperty("applyPileupCorrection",        m_applyPileupCorrection = false);

    ///////// Monitoring Variables
    declareMonitoredStdContainer("EF_etiso_cone2",      m_etiso_cone2, IMonitoredAlgo::AutoClear);
    declareMonitoredStdContainer("EF_etiso_cone3",      m_etiso_cone3, IMonitoredAlgo::AutoClear);
    declareMonitoredStdContainer("EF_etiso_cone4",      m_etiso_cone4, IMonitoredAlgo::AutoClear);
}

// ------------------------------------------------------------------------------------------------------
/**
 * Destructor.
 */
TrigMuonEFCaloIsolation::~TrigMuonEFCaloIsolation() {
}

// ------------------------------------------------------------------------------------------------------
/**
 * Initialize the algorithm.
 */
HLT::ErrorCode TrigMuonEFCaloIsolation::hltInitialize() {

    m_debug   = msgLvl() <= MSG::DEBUG;

    ATH_MSG_DEBUG("Initializing TrigMuonEFCaloIsolation[" << name() << "]");
    ATH_MSG_DEBUG("package version = " << PACKAGE_VERSION);
    ATH_MSG_DEBUG("Properties set as follows: ");
    ATH_MSG_DEBUG("RequireCombinedMuon:       " << m_requireCombined);

    if (m_caloTopoClusterIsolationTool.retrieve().isSuccess()) {
        ATH_MSG_DEBUG("Retrieved " << m_caloTopoClusterIsolationTool);
    } else {
        ATH_MSG_FATAL("Could not retrieve " << m_caloTopoClusterIsolationTool);
        return HLT::BAD_JOB_SETUP;
    }

    ATH_MSG_DEBUG("End of init TrigMuonEFCaloIsolation");

    return HLT::OK;
}

// ------------------------------------------------------------------------------------------------------
/**
 * Fill the et-cone calorimeter isolation values for a container of xAOD muons.
 */
void TrigMuonEFCaloIsolation::fillCaloIsolation(const xAOD::MuonContainer *muons, const xAOD::CaloClusterContainer *clustercont) {

    if (m_caloTopoClusterIsolationTool.empty()) {
        ATH_MSG_WARNING("No calorimeter topo cluster isolation tool available." );
        return;
    }

    std::vector<xAOD::Iso::IsolationType> etCones = { xAOD::Iso::etcone40,
                                                      xAOD::Iso::etcone30,
                                                      xAOD::Iso::etcone20
                                                    };

    for (auto muon : *muons) {

        if (m_debug) {
            ATH_MSG_DEBUG("Processing next EF muon " << muon);
        }

        const xAOD::Muon::MuonType muontype = muon->muonType();
        if ( muontype == xAOD::Muon::MuonType::Combined || muontype == xAOD::Muon::MuonType::SegmentTagged ) {
            if (m_debug) {
                ATH_MSG_DEBUG("EF muon has combined or segment tagged muon");
            }
        } else {
            if (m_requireCombined) {
                if (m_debug) {
                    ATH_MSG_DEBUG("Not a combined or segment tagged muon & requireCombined=true, so ignore this muon");
                }
                continue;
            }//requireCombined
        }//no combined muon

        const xAOD::TrackParticle *tp = 0;
        if (muon->inDetTrackParticleLink().isValid()) tp = *(muon->inDetTrackParticleLink());
        if ( !tp ) tp = muon->primaryTrackParticle();
        if ( !tp ) {
            ATH_MSG_WARNING("No TrackParticle found for muon." );
            continue;
        }

        xAOD::CaloIsolation  caloIsolation;
        xAOD::CaloCorrection corrlist;
        if (m_applyPileupCorrection)
            corrlist.calobitset.set(static_cast<unsigned int>(xAOD::Iso::pileupCorrection));

        corrlist.calobitset.set(static_cast<unsigned int>(xAOD::Iso::IsolationCaloCorrection::coreCone));
        if ( !m_caloTopoClusterIsolationTool->caloTopoClusterIsolation( caloIsolation, *tp, etCones, corrlist, clustercont ) ) {
            ATH_MSG_WARNING("Calculation of topocluster based calorimeter isolation failed");
            continue;
        }
        // if ( !m_caloTopoClusterIsolationTool->decorateParticle_topoClusterIso(*tp, etCones, corrlist, clustercont) ) {
        //         ATH_MSG_WARNING("Calculation of topocluster based calorimeter isolation failed");
        //         continue;
        // }

        for ( unsigned int i = 0; i < etCones.size(); ++i ) {
            ((xAOD::Muon *)muon)->setIsolation(caloIsolation.etcones[i], etCones[i]);
        }

        //Monitor the values
        m_etiso_cone2.push_back( caloIsolation.etcones[2] * 1e-3 );
        m_etiso_cone3.push_back( caloIsolation.etcones[1] * 1e-3 );
        m_etiso_cone4.push_back( caloIsolation.etcones[0] * 1e-3 );

        if (m_debug) {
            ATH_MSG_DEBUG("Filled muon isolation information with:");
            ATH_MSG_DEBUG("\tCone Et 0.2 sum = " << m_etiso_cone2.back() << " GeV");
            ATH_MSG_DEBUG("\tCone Et 0.3 sum = " << m_etiso_cone3.back() << " GeV");
            ATH_MSG_DEBUG("\tCone Et 0.4 sum = " << m_etiso_cone4.back() << " GeV");
        }


    }

}

// ------------------------------------------------------------------------------------------------------
/**
 * Execute function - called for each roi.
 */
HLT::ErrorCode TrigMuonEFCaloIsolation::hltExecute(const HLT::TriggerElement *inputTE, HLT::TriggerElement *TEout) {

    ATH_MSG_DEBUG(": Executing TrigMuonEFCaloIsolation::execHLTAlgorithm()");

    /// extract EF muons
    const xAOD::MuonContainer *muonContainer(0);
    if (HLT::OK != getFeature(inputTE, muonContainer)) {
        ATH_MSG_WARNING("Could not get xAOD::MuonContainer from the trigger element");
        return HLT::MISSING_FEATURE;
    } else {
        if (!muonContainer) {
            ATH_MSG_WARNING("muonContainer is 0 pointer");
            return HLT::MISSING_FEATURE;
        }
        ATH_MSG_DEBUG("MuonContainer extracted with size = " << muonContainer->size());
    }

    //Access the last created trigger cluster container
    std::vector<const xAOD::CaloClusterContainer * > vectorOfClusterContainers;

    if ( getFeatures(inputTE, vectorOfClusterContainers, "") != HLT::OK) { // Was TEout???
        ATH_MSG_ERROR("Failed to get TrigClusters at: getFeatures()");
        return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::NAV_ERROR);
    }

    ATH_MSG_DEBUG("Got vector with " << vectorOfClusterContainers.size() << " ClusterContainers");

    const xAOD::CaloClusterContainer *theClusterCont = 0;
    if ( vectorOfClusterContainers.size() > 0 ) {
        theClusterCont = vectorOfClusterContainers.back();
    } else {
        ATH_MSG_ERROR("Failed to get TrigClusters: vector size is 0");
        return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::NAV_ERROR);
    }

    //For each of the muons fill the calorimeter isolation values using the offline tool
    fillCaloIsolation(muonContainer, theClusterCont);

    //validate sequence
    TEout->setActiveState(true);

    return HLT::OK;
}

// ------------------------------------------------------------------------------------------------------
/**
 * Finalize the algorithm.
 */
HLT::ErrorCode TrigMuonEFCaloIsolation::hltFinalize() {
    ATH_MSG_DEBUG("Finalizing " + name());
    return HLT::OK;
}

/**
 * Here we can handle incidents like end of event, but for now
 * nothing to do.
 */
void TrigMuonEFCaloIsolation::handle(const Incident & /*inc*/) {
}




