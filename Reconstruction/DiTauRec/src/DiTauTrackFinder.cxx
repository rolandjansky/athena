/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DiTauRec/DiTauTrackFinder.h"
#include "DiTauRec/DiTauToolBase.h"

#include "DiTauRec/DiTauCandidateData.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"

#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"

#include "tauRecTools/TrackSort.h"
#include "tauRecTools/KineUtils.h"

#include "fastjet/PseudoJet.hh"


//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------

DiTauTrackFinder::DiTauTrackFinder(const std::string& type,
    const std::string& name,
    const IInterface * parent) :
    DiTauToolBase(type, name, parent),
    m_MaxDrJet(1.0),
    m_MaxDrSubjet(0.2),
    m_TrackParticleContainerName("InDetTrackParticles"),
    m_TrackSelectorTool("")
    // m_ParticleCaloExtensionTool("")
{
    declareInterface<DiTauToolBase > (this);
    declareProperty("MaxDrJet", m_MaxDrJet);
    declareProperty("MaxDrSubjet", m_MaxDrSubjet);
    declareProperty("TrackParticleContainer", m_TrackParticleContainerName);
    declareProperty("TrackSelectorTool", m_TrackSelectorTool);
    // declareProperty("ParticleCaloExtensionTool", m_ParticleCaloExtensionTool);
}

//-------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------

DiTauTrackFinder::~DiTauTrackFinder() {
}

//-------------------------------------------------------------------------
// initialize
//-------------------------------------------------------------------------

StatusCode DiTauTrackFinder::initialize() {

    if (m_TrackSelectorTool.retrieve().isFailure()) {
        ATH_MSG_FATAL("could not retrieve track TrackSelectorTool");
        return StatusCode::FAILURE;
    }
    // if (m_ParticleCaloExtensionTool.retrieve().isFailure()) {
        // ATH_MSG_FATAL("could not retrieve track ParticleCaloExtensionTool");
        // return StatusCode::FAILURE;
    // }

    return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
// Event Finalize
//-------------------------------------------------------------------------

StatusCode DiTauTrackFinder::eventFinalize(DiTauCandidateData * ) {

    return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
// execute
//-------------------------------------------------------------------------

StatusCode DiTauTrackFinder::execute(DiTauCandidateData * data) {

    ATH_MSG_DEBUG("execute DiTauTrackFinder...");

    xAOD::DiTauJet *pDiTau = data->xAODDiTau;

    if (pDiTau == NULL) {
        ATH_MSG_ERROR("no di-tau candidate given");
        return StatusCode::FAILURE;
    }

    StatusCode sc;

    // retrieve track container
    const xAOD::TrackParticleContainer* pTrackParticleCont = 0;
    sc = evtStore()->retrieve(pTrackParticleCont, m_TrackParticleContainerName);
    if (sc.isFailure() || !pTrackParticleCont) {
        ATH_MSG_WARNING("could not find seed jets with key:" << m_TrackParticleContainerName);        
        return StatusCode::FAILURE;
    }

    std::vector<const xAOD::TrackParticle*> tauTracks;    // good tracks in subjets
    std::vector<const xAOD::TrackParticle*> isoTracks;    // good tracks in isolation region
    std::vector<const xAOD::TrackParticle*> otherTracks;  // tracks failing the track criteria

    // get associated primary vertex
    const xAOD::Vertex* pVertex = 0;
    if (pDiTau->vertexLink()!=0) {
        pVertex = (*pDiTau->vertexLink());
    }
    else {
        ATH_MSG_WARNING("could not retieve VertexLink in ditau candidate");
        pVertex = NULL;
    }

    // get tracks
    getTracksFromPV(data, pTrackParticleCont, pVertex, tauTracks, isoTracks, otherTracks);

    // clear track links before association
    pDiTau->clearTrackLinks();
    pDiTau->clearIsoTrackLinks();
    pDiTau->clearOtherTrackLinks();

    // drop subjets without good tracks
    std::vector<fastjet::PseudoJet> vSubjets = data->subjets;
    double nTracks;
    double dR;
    for (auto subjet_itr = vSubjets.begin(); subjet_itr != vSubjets.end(); ) {
        nTracks = 0;
        for (const auto& track : tauTracks) {
            dR = Tau1P3PKineUtils::deltaR(subjet_itr->eta(), subjet_itr->phi_std(), track->eta(), track->phi());
            if (dR < m_MaxDrSubjet) nTracks++;
        }
        ATH_MSG_DEBUG("number of tracks in subjet: "<< nTracks);
        if (nTracks == 0) {
            ATH_MSG_DEBUG("number of tracks is zero. Drop subjet");
            subjet_itr = vSubjets.erase(subjet_itr); //point subjet_itr to the next element/end of the vector
        }
        else {
            ++subjet_itr;
        }
    }
    
    // check if ditau candidate has still at least 2 subjets or 1 subjet plus an electron or muon
    if (vSubjets.size() < 2 ){
        if (vSubjets.size() < 1) {
            ATH_MSG_DEBUG("Found no subjet with track. Reject ditau candidate");
            return StatusCode::FAILURE;
        }
        if (data->electrons.size() < 1 && data->muons.size() < 1) {
            ATH_MSG_DEBUG("Found 1 subjet with track, but no additional electron or muon. Reject ditau candidate");
            return StatusCode::FAILURE;
        }
    }

    data->subjets = vSubjets;
    ATH_MSG_DEBUG("number of subjets  after track association: " << data->subjets.size());
    vSubjets.clear();


    // associate tau tracks
    for (const auto& track : tauTracks ) {
        ATH_MSG_DEBUG("adding subjet track. eta: " << track->eta() << " phi: " << track->phi());
        pDiTau->addTrack(pTrackParticleCont, track);
    }

    // associate isolation tracks
    for (const auto& track : isoTracks ) {
        ATH_MSG_DEBUG("adding iso track. eta: " << track->eta() << " phi: " << track->phi());
        pDiTau->addIsoTrack(pTrackParticleCont, track);
    }

    // associate other tracks
    for (const auto& track : otherTracks ) {
        ATH_MSG_DEBUG("adding other track. eta: " << track->eta() << " phi: " << track->phi());
        pDiTau->addOtherTrack(pTrackParticleCont, track);
    }


    return StatusCode::SUCCESS;
}


void DiTauTrackFinder::getTracksFromPV( const DiTauCandidateData* data,
                                   const xAOD::TrackParticleContainer* pTrackParticleCont,
                                   const xAOD::Vertex* pVertex,
                                   std::vector<const xAOD::TrackParticle*> &tauTracks,
                                   std::vector<const xAOD::TrackParticle*> &isoTracks,
                                   std::vector<const xAOD::TrackParticle*> &otherTracks) {
    
    for (const auto& track : *pTrackParticleCont ) {
        DiTauTrackType type = diTauTrackType(data, track, pVertex);

        if (type == DiTauSubjetTrack)
            tauTracks.push_back(track);
        if (type == DiTauIsoTrack)
            isoTracks.push_back(track);
        if (type == DiTauOtherTrack)
            otherTracks.push_back(track);
    }

    std::sort(tauTracks.begin(), tauTracks.end(), TrackSort());
    std::sort(isoTracks.begin(), isoTracks.end(), TrackSort());
    std::sort(otherTracks.begin(), otherTracks.end(), TrackSort());

}


DiTauTrackFinder::DiTauTrackType DiTauTrackFinder::diTauTrackType(const DiTauCandidateData* data,
                                                        const xAOD::TrackParticle* track,
                                                        const xAOD::Vertex* pVertex) {

    xAOD::DiTauJet *pDiTau = data->xAODDiTau;

    double dRJet = Tau1P3PKineUtils::deltaR(pDiTau->eta(), pDiTau->phi(), track->eta(), track->phi());

    // check if track is outside the jet ditau cone
    if (dRJet > m_MaxDrJet) return OutsideTrack;
    
    // check quality criteria
    bool goodTrack = true;
    goodTrack = m_TrackSelectorTool->decision(*track, pVertex);
    if (!goodTrack) return DiTauOtherTrack;

    // check if track is inside a subjet
    double dR = 100;
    std::vector<fastjet::PseudoJet> vSubjets = data->subjets;
    for (const auto &subjet : vSubjets) {
        dR = Tau1P3PKineUtils::deltaR(subjet.eta(), subjet.phi_std(), track->eta(), track->phi());
        if (dR < m_MaxDrSubjet)
            return DiTauSubjetTrack;
    }

    // track is in isolation region
    return DiTauIsoTrack;
}
