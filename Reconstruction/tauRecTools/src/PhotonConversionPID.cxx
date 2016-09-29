/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
/********************************************************************
NAME:     PhotonConversionPID.cxx
PACKAGE:  offline/Reconstruction/tauRec
AUTHORS:  Michael Boehler <michael.boehler@desy.de>
CREATED:  November 2008
 ********************************************************************/

#include "PhotonConversionPID.h"

#include "xAODTracking/VertexContainer.h" 
//#include "TrkParticleBase/LinkToTrackParticleBase.h"
//#include "TrkTrackSummary/TrackSummary.h"
#include "AthContainers/ConstDataVector.h"

PhotonConversionPID::PhotonConversionPID(const std::string& name)
 :
TauRecToolBase(name),
m_ownPolicy(static_cast<int> (SG::VIEW_ELEMENTS)) {

    declareProperty("OwnPolicy", m_ownPolicy);
    declareProperty("ConversionCandidatesName", m_ConversionCandidatesName = "ConversionCandidate");
    declareProperty("ConversionOutputName", m_ConversionOutputName = "ConversionsPID_Container");
    declareProperty("ElectronProbability", m_eProb_cut = 0.9);
}

/********************************************************************/
PhotonConversionPID::~PhotonConversionPID() {
}

/********************************************************************/
StatusCode PhotonConversionPID::initialize() {
    return StatusCode::SUCCESS;
}

/********************************************************************/
StatusCode PhotonConversionPID::finalize() {
    return StatusCode::SUCCESS;
}

/********************************************************************/
StatusCode PhotonConversionPID::eventFinalize() {

    // ------------------------------------------------------------------ 
    //               Retrieving VxCandidates (conversions)
    // ------------------------------------------------------------------ 
    const xAOD::VertexContainer* ConvContainer;
    StatusCode sc = evtStore()->retrieve(ConvContainer, m_ConversionCandidatesName);

    if (sc.isFailure() || !ConvContainer) {
        ATH_MSG_DEBUG(" No VxCandidates container found in TDS !!");
        return StatusCode::SUCCESS;
    } else {
        ATH_MSG_VERBOSE("Processing Conversion Container Name = " << m_ConversionCandidatesName);
    }

    ATH_MSG_DEBUG("VxContainer " << m_ConversionCandidatesName << " contains " << ConvContainer->size() << " vertices. ");


    // ------------------------------------------------------------------------------------------------- 
    //      Create and record the empty container VxCandidates (conversions after PID)
    // ------------------------------------------------------------------------------------------------- 
    ATH_MSG_DEBUG(" Recording identified conversion collection with key: " << m_ConversionOutputName);

    ConstDataVector<xAOD::VertexContainer>* ConversionContainerPID = new ConstDataVector<xAOD::VertexContainer>(static_cast<SG::OwnershipPolicy> (m_ownPolicy));

    sc = evtStore()->record(ConversionContainerPID, m_ConversionOutputName);
    if (sc.isFailure()) {
        ATH_MSG_ERROR("execute() : cannot record CaloCellContainer " << m_ConversionOutputName);
        return sc;
    }

    // Loop over ConversionVertex Container:
    xAOD::VertexContainer::const_iterator itr = ConvContainer->begin();
    xAOD::VertexContainer::const_iterator itrE = ConvContainer->end();

    for (; itr != itrE; ++itr) {

        const xAOD::Vertex* vxcand = (*itr);

        ATH_MSG_VERBOSE("Tracks at vertex: " << vxcand->nTrackParticles());

        bool isTrk1_Conv = false;
        bool isTrk2_Conv = false;

        // loop over all tracks of the vertex
        for (unsigned int i = 0; i < vxcand->nTrackParticles(); i++) {
            /*
            // these lines navigate from a VxCandidate to the track summary of each track at vertex
            Trk::VxTrackAtVertex* tmpVxAtVtx = (*trklist)[i];
            Trk::ITrackLink* trkLink = tmpVxAtVtx->trackOrParticleLink();
            Trk::LinkToTrackParticleBase * linkToTrack_part = dynamic_cast<Trk::LinkToTrackParticleBase *> (trkLink);
            if (!linkToTrack_part) {
                ATH_MSG_WARNING("dynamic_cast of LinkToTrackParticleBase failed");
                continue;
            }

            const Trk::TrackParticleBase* TP_Base = linkToTrack_part->cachedElement();
            if (!TP_Base) {
                ATH_MSG_WARNING("could not get TrackParticleBase from linkToTrack_part");
                continue;
            }

            const Trk::TrackSummary* summary = TP_Base->trackSummary();
            */
            const xAOD::TrackParticle *track = vxcand->trackParticle(i);
            float eProbabilityHT;
            if (track && track->summaryValue(eProbabilityHT, xAOD::SummaryType::eProbabilityHT)) {
                //---------------------------------------------------------------
                // Checks ID of Conversion Candidates (eProb based on TRT PID)
                ATH_MSG_VERBOSE("Track " << i + 1 << "  PID: " << eProbabilityHT);

                if (eProbabilityHT > m_eProb_cut) {

                    // both conv tracks pass PID cut (in case of double track conv)
                    if (i == 0) {
                        isTrk1_Conv = true;
                    }
                    if (i == 1 || vxcand->nTrackParticles() == 1) {
                        isTrk2_Conv = true;
                    }
                }

                //------------------------------------------------------------------
                // after both tracks have been tested and passed the PID cut the 
                // Conversions are stored in the new Conversion Container
                if (isTrk1_Conv == true && isTrk2_Conv == true) {
                    ATH_MSG_VERBOSE("                   Conversion accepted");
                    ConversionContainerPID->push_back(vxcand);
                }


            } else {
                // here a new track summary can be provided (not yet implemented)
                ATH_MSG_WARNING("No Track Summary has been found! NO Photon Conversion Identification has been made!!!! ");
            }

        } // end track loop
    } // end vertex loop

    ATH_MSG_DEBUG("After ID " << ConversionContainerPID->size() << " Conversion are stored in " << m_ConversionOutputName);

    return StatusCode::SUCCESS;
}

/********************************************************************/
#endif
