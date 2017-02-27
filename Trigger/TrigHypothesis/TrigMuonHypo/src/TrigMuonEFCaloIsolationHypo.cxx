/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonHypo/TrigMuonEFCaloIsolationHypo.h"

#include "xAODMuon/MuonContainer.h"

#include "xAODTrigger/TrigPassBits.h"

class ISvcLocator;

/**
 * Constructor.
 * Declare variables to get from job options
 * the monitored variables.
 */
TrigMuonEFCaloIsolationHypo::TrigMuonEFCaloIsolationHypo(const std::string &name, ISvcLocator *pSvcLocator):
    HLT::HypoAlgo(name, pSvcLocator) {

    declareProperty("AcceptAll", m_acceptAll = true);
    declareProperty("DoAbsCut", m_abscut = true); //true for absolute cuts, false for sumet/pt
    // declareMonitoredStdContainer("PtCone02", m_fex_ptcone02,  IMonitoredAlgo::AutoClear);
    // declareMonitoredStdContainer("PtCone03", m_fex_ptcone03,  IMonitoredAlgo::AutoClear);

    //Std isolation
    declareProperty("CaloConeSize",         m_CaloConeSize = 1); //Calorimetric cone size (1: DR=0.2, 2: DR=0.3: 3: DR=0.4)
    m_CaloConeSize.verifier().setBounds(1, 4);

    declareProperty("MaxCaloIso_1",         m_MaxCaloIso_1 = 4000.0);
    declareProperty("MaxCaloIso_2",         m_MaxCaloIso_2 = 4000.0);
    declareProperty("MaxCaloIso_3",         m_MaxCaloIso_3 = 3500.0);

    declareProperty("EtaRegion_1",          m_EtaRegion_1 = 1.0);
    declareProperty("EtaRegion_2",          m_EtaRegion_2 = 1.4);
    declareProperty("EtaRegion_3",          m_EtaRegion_3 = 9.9);

    declareMonitoredVariable("CutCounter",  m_cutCounter);
    declareMonitoredVariable("SumEtCone",   m_SumEtCone);
}

/**
 * Destructor. Nothing to do for now.
 */
TrigMuonEFCaloIsolationHypo::~TrigMuonEFCaloIsolationHypo() {

}

/**
 * Initialize the algorithm.
 * Here we print out the cut values.
 */
HLT::ErrorCode TrigMuonEFCaloIsolationHypo::hltInitialize() {

    if (m_acceptAll) {
        ATH_MSG_INFO("Accepting all the events with no cut!");
    } else {
        ATH_MSG_INFO("Calo based isolation activated");
        ATH_MSG_INFO( (m_abscut ? "Absolute" : "Relative") << " isolation cut for calo selected");
        ATH_MSG_INFO("Calorimetric cone size selected: " << m_CaloConeSize);
	ATH_MSG_INFO("MaxCaloIso_1/2/3 = "<<m_MaxCaloIso_1<<"/"<<m_MaxCaloIso_2<<"/"<<m_MaxCaloIso_3);
        if (m_MaxCaloIso_1 < 0.0 && m_MaxCaloIso_2 < 0.0 && m_MaxCaloIso_3 < 0.0) {
            ATH_MSG_FATAL("Configured to apply cuts, but not cut was specified");
            return HLT::BAD_JOB_SETUP;
        }
    }

    ATH_MSG_INFO("Initialization completed successfully");

    return HLT::OK;
}

/**
 * Finalize the algorithm - nothing to do here yet.
 */
HLT::ErrorCode TrigMuonEFCaloIsolationHypo::hltFinalize() {
    return HLT::OK;
}

/**
 * Execute the algorithm.
 * Here we apply the cuts to the muon(s) and their isolation properties.
 * The hypo passes if at least one muon passes the cuts.
 */
HLT::ErrorCode TrigMuonEFCaloIsolationHypo::hltExecute(const HLT::TriggerElement *outputTE, bool &pass) {

    ATH_MSG_DEBUG("in execute()");

    //reset monitored variables
    m_SumEtCone = 0.0;
    m_cutCounter = 0;

    if (m_acceptAll) {
        pass = true;
        ATH_MSG_DEBUG("Accept property is set: all events accepted");
        return HLT::OK;
    }//acceptAll

    pass = false; // fail by default

    // Some debug output
    ATH_MSG_DEBUG("outputTE->ID(): " << outputTE->getId());

    // Get the muon container from the outputTE
    const xAOD::MuonContainer *muonContainer(0);
    if (getFeature(outputTE, muonContainer) != HLT::OK || muonContainer == 0) {
        ATH_MSG_DEBUG("no MuonContainer Feature found");
        return HLT::MISSING_FEATURE;
    }

    // make pass bits object to store the result per muon
    std::unique_ptr<xAOD::TrigPassBits> xBits = xAOD::makeTrigPassBits<xAOD::MuonContainer>(muonContainer);

    //Select the right isolation variable
    xAOD::Iso::IsolationType selectedIsoCone;
    if      (m_CaloConeSize == 1) selectedIsoCone  = xAOD::Iso::etcone20;
    else if (m_CaloConeSize == 2) selectedIsoCone  = xAOD::Iso::etcone30;
    else if (m_CaloConeSize == 3) selectedIsoCone  = xAOD::Iso::etcone40;
    else                          selectedIsoCone  = xAOD::Iso::etcone20;

    // loop over objects (muons) in the container
    for (auto muon : *muonContainer) {

        float measuredEtCone(-1);
        bool isIsolated = false;

        bool res = muon->isolation(measuredEtCone, selectedIsoCone);
        if (!res)
            ATH_MSG_WARNING("Problem accessing etcone variable, " << measuredEtCone);

        const double mupt = muon->pt();
        const double mueta = muon->eta();
        ATH_MSG_DEBUG("Muon with pT " << mupt << " and eta " << muon->eta());

        //For monitoring
        m_SumEtCone = measuredEtCone / 1000.0;

        if (!m_abscut) {
            //relative cut needed
            if (mupt > 0) measuredEtCone /= mupt;
        }

        if (fabs(mueta) <= m_EtaRegion_1) {

            if (measuredEtCone <= m_MaxCaloIso_1.value()) {
                isIsolated = true;
                m_cutCounter = 1;
            }

        } else if (fabs(mueta) > m_EtaRegion_1 && fabs(mueta) <= m_EtaRegion_2 ) {

            if (measuredEtCone <= m_MaxCaloIso_2.value()) {
                isIsolated = true;
                m_cutCounter = 2;
            }

        } else if (fabs(mueta) > m_EtaRegion_2 && fabs(mueta) <= m_EtaRegion_3 ) {

            if (measuredEtCone <= m_MaxCaloIso_3.value()) {
                isIsolated = true;
                m_cutCounter = 3;
            }
        }

        ATH_MSG_DEBUG(" REGTEST usealgo / pt / eta / SumEtCone : "
                      << " / " << mupt / 1000.0
                      << " / " << mueta
                      << " / " << m_SumEtCone
                      << " / Muon Isolation Hypotesis is " << (isIsolated ? "true" : "false"));


        if (isIsolated) {
	    xBits->markPassing(muon,muonContainer,true); // set bit for this muon in TrigPassBits mask
            pass = true;
        }//muon passed
    }//loop over all muons

    ATH_MSG_DEBUG("Algo result = " << (pass ? "true" : "false"));

    // store TrigPassBits result
    if ( attachFeature(outputTE, xBits.release(),"passbits") != HLT::OK ) {
        ATH_MSG_ERROR("Could not store TrigPassBits!");
    }

    return HLT::OK;
}//hltExecute



