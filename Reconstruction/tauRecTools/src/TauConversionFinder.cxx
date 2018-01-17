/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
/********************************************************************
NAME:     TauConversionFinder.cxx
PACKAGE:  offline/Reconstruction/tauRec
AUTHORS:  Michael Boehler <michael.boehler@desy.de>
CREATED:  November 2008

This tool identifies if a tau track is reconstructed as photon 
conversion track too.
 ********************************************************************/

#include "TauConversionFinder.h"

#include "xAODTracking/VertexContainer.h" 
//#include "GaudiKernel/IToolSvc.h"
//#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkTrackSummary/TrackSummary.h"

/********************************************************************/
TauConversionFinder::TauConversionFinder(const std::string& name) :
  TauRecToolBase(name),
  m_numProng(0)
{

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
StatusCode TauConversionFinder::eventFinalize() {

    StatusCode sc;

//    const Rec::TrackParticleContainer *trackContainer;
    const xAOD::TrackParticleContainer* trackContainer = 0;

    //TODO: trigger uses getObject
    sc = evtStore()->retrieve(trackContainer, m_trackContainerName);
    if (sc.isFailure() || !trackContainer) {
        ATH_MSG_DEBUG(" No track container found in TDS !!");
        return StatusCode::SUCCESS;
    }

    // ------------------------------------------------------------------ 
    //               Retrieving VxCandidates (conversions)
    // ------------------------------------------------------------------   
    const xAOD::VertexContainer* ConvContainer =  nullptr;

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
    xAOD::TauJetContainer *pTauJetCont = tauEventData()->xAODTauContainer;

    for (xAOD::TauJetContainer::iterator tjcItr = pTauJetCont->begin(); tjcItr != pTauJetCont->end(); ++tjcItr) {
        
        xAOD::TauJet& pTau = **tjcItr;

        unsigned int numTracks = pTau.nTracks();

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
		        //const xAOD::TrackParticle *pTauTrack = pTau.track(j);
		        xAOD::TauTrack *pTauTrack = pTau.trackNonConst(j);
		        const Trk::Track* tau_trk_def = pTauTrack->track()->track();

                        if (conv_trk == tau_trk_def) {

                            if (conv_trk->trackSummary()->getPID(Trk::eProbabilityHT) > m_eProb_cut) {
                              //if (!pTau.trackFlag(pTauTrack, xAOD::TauJetParameters::isConversion)) {
                              if (!pTauTrack->flag(xAOD::TauJetParameters::isConversionOld)) {
                                  ElementLink<xAOD::TrackParticleContainer> phoConvLink ;
                                  //phoConvLink.setElement(pTauTrack) ;
				  phoConvLink.setElement(pTauTrack->track()) ;
                                  phoConvLink.setStorableObject( *trackContainer ) ;
                                  //phoConvLink.index();
				  pTauTrack->addTrackLink( phoConvLink );
				  pTauTrack->setFlag(xAOD::TauJetParameters::isConversionOld, true);
                                    if (m_adjust_tau_charge)
				      pTau.setCharge(pTau.charge() - pTau.track(j)->track()->charge());

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
#endif
