/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
NAME:     TauConversionFinder.cxx
PACKAGE:  offline/Reconstruction/tauRec
AUTHORS:  Michael Boehler <michael.boehler@desy.de>
CREATED:  November 2008

This tool identifies if a tau track is reconstructed as photon 
conversion track too.
 ********************************************************************/

#include "tauRec/TauConversionFinder.h"

#include "xAODTracking/VertexContainer.h" 
#include "GaudiKernel/IToolSvc.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"

/********************************************************************/
TauConversionFinder::TauConversionFinder(const std::string& type,
    const std::string& name,
    const IInterface* parent) :
TauToolBase(type, name, parent) {
    declareInterface<TauToolBase > (this);

    declareProperty("ConversionCandidatesName", m_ConversionCandidatesName = "ConversionsPID_Container"); //ConversionCandidate
    declareProperty("TrackContainerName", m_trackContainerName = "InDetTrackParticles");
    declareProperty("DoNormalTracks", m_do_normal = true);
    declareProperty("MinElectronProbability", m_eProb_cut = 0.9);
    declareProperty("AdjustTauCharge", m_adjust_tau_charge = false);

}

/********************************************************************/
TauConversionFinder::~TauConversionFinder() {
}

/********************************************************************/
StatusCode TauConversionFinder::initialize() {
    ATH_MSG_VERBOSE("TauConversionFinder Initialising");

    return StatusCode::SUCCESS;
}

/********************************************************************/
StatusCode TauConversionFinder::finalize() {
    ATH_MSG_VERBOSE("TauConversionFinder Finalizing");

    return StatusCode::SUCCESS;
}

/********************************************************************/
StatusCode TauConversionFinder::eventFinalize(TauCandidateData *data) {

    StatusCode sc;

    const Rec::TrackParticleContainer *trackContainer;

    //TODO: trigger uses getObject
    sc = evtStore()->retrieve(trackContainer, m_trackContainerName);
    if (sc.isFailure() || !trackContainer) {
        ATH_MSG_DEBUG(" No track container found in TDS !!");
        return StatusCode::SUCCESS;
    }

    // ------------------------------------------------------------------ 
    //               Retrieving VxCandidates (conversions)
    // ------------------------------------------------------------------   
    const xAOD::VertexContainer* ConvContainer;

    sc = evtStore()->retrieve(ConvContainer, m_ConversionCandidatesName);
    if (sc.isFailure() || !ConvContainer) {
        ATH_MSG_DEBUG(" No VxCandidates container found in TDS !!");
        return StatusCode::SUCCESS;
    } else {
        ATH_MSG_VERBOSE("Processing Conversion Container Name = " << m_ConversionCandidatesName);
    }
    ATH_MSG_VERBOSE("VxContainer " << m_ConversionCandidatesName << " contains " << ConvContainer->size() << " vertices. ");

    // ------------------------------------------------------------------ 
    //                    Check number of Tau Tracks
    // ------------------------------------------------------------------ 
    if (!m_do_normal) { 
        return StatusCode::SUCCESS;
    }

    // running in eventFinalize
    // therefore need to loop over all tau candidates 
    xAOD::TauJetContainer *pTauJetCont = data->xAODTauContainer;
    for (xAOD::TauJetContainer::iterator tjcItr = pTauJetCont->begin(); tjcItr != pTauJetCont->end(); ++tjcItr) {
        
        xAOD::TauJet *pTau = *tjcItr;
        if (pTau == NULL) {
            ATH_MSG_DEBUG("no candidate given");
            continue;
        }

        unsigned int numTracks = pTau->nTracks();

        m_numProng = numTracks;

        if (m_do_normal)
            ATH_MSG_VERBOSE("Number of tau tracks before ConversionFinder (TauJet): " << m_numProng);

        // Loop over Conversion Container placed by TauPhotonConversionFinder
        xAOD::VertexContainer::const_iterator itr = ConvContainer->begin();
        xAOD::VertexContainer::const_iterator itrE = ConvContainer->end();
        for (; itr != itrE; ++itr) {

            const xAOD::Vertex* vxcand = (*itr);

            for (unsigned int i = 0; i < vxcand->nTrackParticles(); ++i) {

                const Trk::Track* conv_trk = vxcand->trackParticle(i)->track();
                // just check if the track is reconstructed only by TRT
                //--------------------------------------------
                // Find conversion in normal tau tracks
                if (m_do_normal) {
                    for (unsigned int j = 0; j < numTracks; ++j) {
                        const xAOD::TrackParticle *pTauTrack = pTau->track(j);
                        const Trk::Track* tau_trk_def = pTauTrack->track();

                        if (conv_trk == tau_trk_def) {
                            if (conv_trk->trackSummary()->getPID(Trk::eProbabilityComb) > m_eProb_cut) {
                                bool isConversionTrack = false;
                                for (unsigned int k = 0; k < pTau->nConversionTracks(); k++) {
                                    if (pTau->conversionTrack(k)->track() == pTau->track(j)->track())
                                        isConversionTrack = true;
                                }
                                if (isConversionTrack) {
                                    pTau->addConversionTrackLink(pTau->conversionTrackLinks().at(j));
                                    if (m_adjust_tau_charge)
                                        pTau->setCharge(pTau->charge() - pTau->track(j)->charge());

                                    m_numProng--;
                                }
                            }
                        }
                    }
                }
            }//end of loop over Tracks at vertex
        }// end of loop over VxContainer

        if (m_do_normal)
            ATH_MSG_VERBOSE("Number of tau tracks after ConversionFinder (TauJet): " << m_numProng);
    }// end of loop over TauJetContainer

    return StatusCode::SUCCESS;
}

/********************************************************************/
