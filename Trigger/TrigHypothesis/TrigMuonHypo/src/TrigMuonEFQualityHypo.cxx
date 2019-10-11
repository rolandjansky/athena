/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonHypo/TrigMuonEFQualityHypo.h"

#include "xAODMuon/MuonContainer.h"

#include "xAODTrigger/TrigPassBits.h"

class ISvcLocator;

/**
 * Constructor.
 * Declare variables to get from job options
 * the monitored variables.
 */
TrigMuonEFQualityHypo::TrigMuonEFQualityHypo(const std::string &name, ISvcLocator *pSvcLocator):
    HLT::HypoAlgo(name, pSvcLocator),
    m_muonSelTool("CP::MuonSelectionTool/MuonSelectionTool")
 {

    declareProperty("AcceptAll", m_acceptAll = true);

    declareProperty("MuonSelectionTool", m_muonSelTool);    

    // kept in case monitor is needed
    declareMonitoredVariable("isBadMuon",  m_isBadMuon);
    declareMonitoredVariable("reducedChi2",   m_reducedChi2);

    declareProperty("LooseCut", m_looseCut = true);

}

/**
 * Destructor. Nothing to do for now.
 */
TrigMuonEFQualityHypo::~TrigMuonEFQualityHypo() {

}

/**
 * Initialize the algorithm.
 * Here we print out the cut values.
 */
HLT::ErrorCode TrigMuonEFQualityHypo::hltInitialize() {

    if(m_muonSelTool.retrieve().isFailure()) ATH_MSG_INFO("Unable to retrieve " << m_muonSelTool);

    if (m_acceptAll) {
        ATH_MSG_INFO("Accepting all the events with no cut!");
    } else {
        ATH_MSG_INFO("Muon quality criteria required");
        if(m_looseCut) ATH_MSG_INFO(" Applied chi2 requirement ");
        else ATH_MSG_INFO("chi2 + additional flags required "); 
    }

    ATH_MSG_INFO("Initialization completed successfully");

    return HLT::OK;
}

/**
 * Finalize the algorithm - nothing to do here yet.
 */
HLT::ErrorCode TrigMuonEFQualityHypo::hltFinalize() {
    return HLT::OK;
}

/**
 * Execute the algorithm.
 * Here we apply the quality cuts to the muon(s).
 */

HLT::ErrorCode TrigMuonEFQualityHypo::hltExecute(const HLT::TriggerElement *outputTE, bool &pass) {

    ATH_MSG_DEBUG("in execute()");

    //reset monitored variables
    m_reducedChi2 = -10.0;
    m_isBadMuon = 0;
    
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

    // loop over objects (muons) in the container
    for (auto muon : *muonContainer) {

        bool passCut = false;

        const double mupt = muon->pt();
        const double mueta = muon->eta();
        ATH_MSG_DEBUG("Muon with pT " << mupt << " and eta " << muon->eta());

        m_isBadMuon = m_muonSelTool->isBadMuon(*muon);
   
        const xAOD::TrackParticle* idtrack = muon->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
        const xAOD::TrackParticle* metrack = muon->trackParticle( xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle );
        float mePt = -999999., idPt = -999999.;

        float reducedChi2 = -10, qOverPsignif = -10;

        if(idtrack && metrack) {
            mePt = metrack->pt();
            idPt = idtrack->pt();

            float meP  = 1.0 / ( sin(metrack->theta()) / mePt); 
            float idP  = 1.0 / ( sin(idtrack->theta()) / idPt); 
            qOverPsignif  = fabs( (metrack->charge() / meP) - (idtrack->charge() / idP) ) / sqrt( idtrack->definingParametersCovMatrix()(4,4) + metrack->definingParametersCovMatrix()(4,4) ); 
            reducedChi2 = muon->primaryTrackParticle()->chiSquared()/muon->primaryTrackParticle()->numberDoF(); 
        }
        m_reducedChi2 = reducedChi2;
        m_isBadMuon = !m_muonSelTool->isBadMuon(*muon);

        if(m_looseCut) {
            if(fabs(reducedChi2) < 8.0) passCut = true;
        } else {
            if(fabs(reducedChi2) < 8.0 && !m_muonSelTool->isBadMuon(*muon) && fabs(qOverPsignif)<7.0 && muon->author()==xAOD::Muon::MuidCo) passCut = true;
        }

        ATH_MSG_DEBUG(" REGTEST usealgo / pt / eta / reducedChi2 / Quality flags : "
                      << " / " << mupt / 1000.0
                      << " / " << mueta
                      << " / " << m_reducedChi2
                      << " / " << m_isBadMuon  
                      << " / Muon Quality Hypothesis is " << (passCut ? "true" : "false"));


        if (passCut) {
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



