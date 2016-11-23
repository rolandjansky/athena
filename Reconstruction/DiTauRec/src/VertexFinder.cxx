/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DiTauRec/VertexFinder.h"
#include "DiTauRec/DiTauToolBase.h"

#include "DiTauRec/DiTauCandidateData.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"

//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------

VertexFinder::VertexFinder(const std::string& type,
    const std::string& name,
    const IInterface * parent) :
    DiTauToolBase(type, name, parent),
    m_primVtxContainerName("PrimaryVertices"),
    m_assocTracksName("GhostTrack"),
    m_trackVertexAssocName("JetTrackVtxAssoc_forDiTaus"),
    m_maxJVF(-100)
{
    declareInterface<DiTauToolBase > (this);
    declareProperty("PrimVtxContainerName", m_primVtxContainerName);
    declareProperty("AssociatedTracks", m_assocTracksName);
    declareProperty("TrackVertexAssociation", m_trackVertexAssocName);
}

//-------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------

VertexFinder::~VertexFinder() {
}

//-------------------------------------------------------------------------
// initialize
//-------------------------------------------------------------------------

StatusCode VertexFinder::initialize() {

    return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
// Event Finalize
//-------------------------------------------------------------------------

StatusCode VertexFinder::eventFinalize(DiTauCandidateData * ) {

    return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
// execute
//-------------------------------------------------------------------------

StatusCode VertexFinder::execute(DiTauCandidateData * data) {

    ATH_MSG_DEBUG("execute VertexFinder...");

    xAOD::DiTauJet *pDiTau = data->xAODDiTau;
  
    // get the primary vertex container from StoreGate
    //do it here because of tau trigger
    const xAOD::VertexContainer* vxContainer = 0;
    const xAOD::Vertex* vxPrimary = 0;
  
    StatusCode sc = evtStore()->retrieve(vxContainer, m_primVtxContainerName);
    if (sc.isFailure() || vxContainer->size() == 0) {
        ATH_MSG_WARNING("Container (" << m_primVtxContainerName << 
                        ") not found in StoreGate.");
        return StatusCode::FAILURE;
    }

    // find default PrimaryVertex
    // see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/VertexReselectionOnAOD
    // and https://svnweb.cern.ch/trac/atlasoff/browser/Tracking/TrkEvent/VxVertex/trunk/VxVertex/PrimaryVertexSelector.h
    for (const auto & vtx : *vxContainer) {
        // the first and only primary vertex candidate is picked
        if ( vtx->vertexType() ==  xAOD::VxType::PriVtx) {
          vxPrimary = vtx;
          break;
        }
    }


    if (vxPrimary == 0) {
        ATH_MSG_WARNING("Unable to find primary Vertex in VertexContainer." << 
                        " Continue without Tau Vertex Association.");
        return StatusCode::SUCCESS;
    }
        
    // associate vertex to tau
    pDiTau->setVertex(vxContainer, vxPrimary);
       

    // try to find new PV with TJVA
    ATH_MSG_DEBUG("TJVA enabled -> try to find new PV for the tau candidate");

    ElementLink<xAOD::VertexContainer> newPrimaryVertexLink = getPV_TJVA(pDiTau, vxContainer);
    if (newPrimaryVertexLink.isValid()) {
        // set new primary vertex
        // will overwrite default one which was set above
        pDiTau->setVertexLink(newPrimaryVertexLink);
        // save highest JVF value
        pDiTau->setDetail(xAOD::DiTauJetParameters::TauJetVtxFraction,static_cast<float>(m_maxJVF));
        ATH_MSG_DEBUG("TJVA vertex found and set");
    }
    else {
        ATH_MSG_DEBUG("couldn't find new PV for TJVA");
    }

    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
ElementLink<xAOD::VertexContainer> VertexFinder::getPV_TJVA(const xAOD::DiTauJet* pDiTau, const xAOD::VertexContainer* vertices)
{
    const xAOD::Jet* pJetSeed = (*pDiTau->jetLink());

    // the implementation follows closely the example given in modifyJet(...) in https://svnweb.cern.ch/trac/atlasoff/browser/Reconstruction/Jet/JetMomentTools/trunk/Root/JetVertexFractionTool.cxx#15

    // Get the tracks associated to the jet
    std::vector<const xAOD::TrackParticle*> assocTracks;
    if (! pJetSeed->getAssociatedObjects(m_assocTracksName, assocTracks)) {
        ATH_MSG_ERROR("Could not retrieve the AssociatedObjects named \""<< m_assocTracksName <<"\" from jet");
        return ElementLink<xAOD::VertexContainer>();
    }

    // Get the TVA object
    const jet::TrackVertexAssociation* tva = NULL;
    if (evtStore()->retrieve(tva, m_trackVertexAssocName).isFailure()) {
        ATH_MSG_ERROR("Could not retrieve the TrackVertexAssociation from evtStore: " << m_trackVertexAssocName);
        return ElementLink<xAOD::VertexContainer>();
    }

    // Calculate Jet Vertex Fraction
    std::vector<float> jvf;
    jvf.resize(vertices->size());
    for (size_t iVertex = 0; iVertex < vertices->size(); ++iVertex) {
        jvf.at(iVertex) = getJetVertexFraction(vertices->at(iVertex),assocTracks,tva);
    }
    
    // Get the highest JVF vertex and store maxJVF for later use
    // Note: the official JetMomentTools/JetVertexFractionTool doesn't provide any possibility to access the JVF value, but just the vertex.
    m_maxJVF=-100.;
    size_t maxIndex = 0;
    for (size_t iVertex = 0; iVertex < jvf.size(); ++iVertex) {
        if (jvf.at(iVertex) > m_maxJVF) {
            m_maxJVF = jvf.at(iVertex);
            maxIndex = iVertex;
        }
    }

    // Set the highest JVF vertex
    ElementLink<xAOD::VertexContainer> vtxlink = ElementLink<xAOD::VertexContainer>(*vertices,vertices->at(maxIndex)->index());

    return vtxlink;
}

// reimplementation of JetVertexFractionTool::getJetVertexFraction(const xAOD::Vertex* vertex, const std::vector<const xAOD::TrackParticle*>& tracks, const jet::TrackVertexAssociation* tva) const
// avoid to call this specific tool only for this easy purpose
// see https://svnweb.cern.ch/trac/atlasoff/browser/Reconstruction/Jet/JetMomentTools/trunk/Root/JetVertexFractionTool.cxx
float VertexFinder::getJetVertexFraction(const xAOD::Vertex* vertex, const std::vector<const xAOD::TrackParticle*>& tracks, const jet::TrackVertexAssociation* tva) const
{
    float sumTrackPV = 0;
    float sumTrackAll = 0;
    for (size_t iTrack = 0; iTrack < tracks.size(); ++iTrack)
    {
        const xAOD::TrackParticle* track = tracks.at(iTrack);
        const xAOD::Vertex* ptvtx = tva->associatedVertex(track);
        if (ptvtx != nullptr) {  // C++11 feature
            if (ptvtx->index() == vertex->index()) sumTrackPV += track->pt();
        }
        sumTrackAll += track->pt();

    }
    return sumTrackAll!=0 ? sumTrackPV/sumTrackAll : 0;
}
