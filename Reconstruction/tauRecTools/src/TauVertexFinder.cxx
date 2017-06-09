/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
#include "TauVertexFinder.h"

#include "VxVertex/RecVertex.h"
#include "VxVertex/VxCandidate.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauJet.h"

TauVertexFinder::TauVertexFinder(const std::string& name ) :
  TauRecToolBase(name),
  m_printMissingContainerINFO(true),
  m_TrackSelectionToolForTJVA(""),
  m_assocTracksName(""),
  m_trackVertexAssocName("")
{
  declareProperty("UseTJVA", m_useTJVA=true);
  declareProperty("PrimaryVertexContainer", m_inputPrimaryVertexContainerName = "PrimaryVertices");
  declareProperty("AssociatedTracks",m_assocTracksName);
  declareProperty("TrackVertexAssociation",m_trackVertexAssocName);
  declareProperty("InDetTrackSelectionToolForTJVA",m_TrackSelectionToolForTJVA);
 
  // online ATR-15665 
  declareProperty("OnlineMaxTransverseDistance",m_transDistMax=10e6);
  declareProperty("OnlineMaxLongitudinalDistance",m_longDistMax=10e6);
  declareProperty("OnlineMaxZ0SinTheta",m_maxZ0SinTheta=10e6);

}

TauVertexFinder::~TauVertexFinder() {
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode TauVertexFinder::initialize() {
  if (m_useTJVA) ATH_MSG_INFO("using TJVA to determine tau vertex");
  ATH_CHECK(retrieveTool(m_TrackSelectionToolForTJVA));
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode TauVertexFinder::finalize() {
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode TauVertexFinder::eventInitialize() {
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode TauVertexFinder::eventFinalize() {
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode TauVertexFinder::execute(xAOD::TauJet& pTau) {
  
  // get the primary vertex container from StoreGate
  //do it here because of tau trigger
  const xAOD::VertexContainer* vxContainer = 0;
  const xAOD::Vertex* primaryVertex = 0;
 
  StatusCode sc;
  //for tau trigger
  sc = tauEventData()->getObject("VxPrimaryCandidate", vxContainer);
  
  if (sc.isFailure() || !vxContainer) { //not in trigger mode or no vxContainer was set by trigger
    ATH_MSG_DEBUG("no VxPrimaryCandidateContainer for trigger -> try standard way");
    if (!openContainer(vxContainer, m_inputPrimaryVertexContainerName)) {
      if (m_printMissingContainerINFO) {
	ATH_MSG_INFO(m_inputPrimaryVertexContainerName << " container not found --> skip TauVertexFinder (no further info)");
	m_printMissingContainerINFO=false;
      }
      return StatusCode::SUCCESS;
    }

    // find default PrimaryVertex (needed if TJVA is switched off or fails)
    // see: https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/VertexReselectionOnAOD
    // code adapted from 
    // https://svnweb.cern.ch/trac/atlasoff/browser/Tracking/TrkEvent/VxVertex/trunk/VxVertex/PrimaryVertexSelector.h
    if (vxContainer->size()>0) {   
      // simple loop through and get the primary vertex
      xAOD::VertexContainer::const_iterator vxIter    = vxContainer->begin();
      xAOD::VertexContainer::const_iterator vxIterEnd = vxContainer->end();
      for ( size_t ivtx = 0; vxIter != vxIterEnd; ++vxIter, ++ivtx ){
	// the first and only primary vertex candidate is picked
	if ( (*vxIter)->vertexType() ==  xAOD::VxType::PriVtx){
	  primaryVertex = (*vxIter);
	  break;
	}
      }
    }
  }
  else { // trigger mode
    // find default PrimaryVertex (highest sum pt^2)
    if (vxContainer->size()>0) primaryVertex = (*vxContainer)[0];
  }
    
  ATH_MSG_VERBOSE("size of VxPrimaryContainer is: "  << vxContainer->size() );
    
  // associate vertex to tau
  if (primaryVertex) pTau.setVertex(vxContainer, primaryVertex);
       
  //stop here if TJVA is disabled or vertex container is empty
  if (!m_useTJVA || vxContainer->size()==0) return StatusCode::SUCCESS;

  // try to find new PV with TJVA
  ATH_MSG_DEBUG("TJVA enabled -> try to find new PV for the tau candidate");

  float maxJVF = -100;
  ElementLink<xAOD::VertexContainer> newPrimaryVertexLink =
    getPV_TJVA(pTau, *vxContainer, maxJVF );
  if (newPrimaryVertexLink.isValid()) {
    // set new primary vertex
    // will overwrite default one which was set above
    pTau.setVertexLink(newPrimaryVertexLink);
    // save highest JVF value
    pTau.setDetail(xAOD::TauJetParameters::TauJetVtxFraction,static_cast<float>(maxJVF));
    ATH_MSG_DEBUG("TJVA vertex found and set");
  }
  else {
    ATH_MSG_DEBUG("couldn't find new PV for TJVA");
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
ElementLink<xAOD::VertexContainer>
TauVertexFinder::getPV_TJVA(const xAOD::TauJet& pTau,
                            const xAOD::VertexContainer& vertices,
                            float& maxJVF)
{
  const xAOD::Jet* pJetSeed = (*pTau.jetLink());
  std::vector<const xAOD::TrackParticle*> tracksForTJVA;
  const double dDeltaRMax(0.2);

  m_matchedVertexOnline.clear(); 
  // the implementation follows closely the example given in modifyJet(...) in https://svnweb.cern.ch/trac/atlasoff/browser/Reconstruction/Jet/JetMomentTools/trunk/Root/JetVertexFractionTool.cxx#15

  // Get the tracks associated to the jet
  std::vector<const xAOD::TrackParticle*> assocTracks;

  //for tau trigger (ATR-15665)
  bool inTrigger = tauEventData()->inTrigger();
  const xAOD::TrackParticleContainer* trackParticleCont = 0;
  if (inTrigger)   {
     StatusCode sc = tauEventData()->getObject( "TrackContainer", trackParticleCont ); // to be replaced by full FTK collection?
     if (sc.isFailure() || !trackParticleCont){
        ATH_MSG_WARNING("No TrackContainer for TJVA in trigger found");
        return ElementLink<xAOD::VertexContainer>();
     }
     // convert TrackParticleContainer in std::vector<const xAOD::TrackParticle*>
     for (xAOD::TrackParticleContainer::const_iterator tpcItr = trackParticleCont->begin(); tpcItr != trackParticleCont->end(); ++tpcItr) {
         const xAOD::TrackParticle *trackParticle = *tpcItr;
         assocTracks.push_back(trackParticle);
     }
     ATH_MSG_DEBUG("TrackContainer for online TJVA with size "<< assocTracks.size()); 
  }
  else {
    if (! pJetSeed->getAssociatedObjects(m_assocTracksName, assocTracks)) {
       ATH_MSG_ERROR("Could not retrieve the AssociatedObjects named \""<< m_assocTracksName <<"\" from jet");
       return ElementLink<xAOD::VertexContainer>();
    }
  }

  // Store tracks that meet TJVA track selection criteria and are between deltaR of 0.2 with the jet seed
  // To be included in the TJVA calculation
  // Maybe not as efficient as deleting unwanted tracks from assocTrack but quicker and safer for now.
  for ( auto xTrack : assocTracks ){
    if ( (xTrack->p4().DeltaR(pJetSeed->p4())<dDeltaRMax) && m_TrackSelectionToolForTJVA->accept(*xTrack) )
      tracksForTJVA.push_back(xTrack);
  }

  // Get the TVA object
  const jet::TrackVertexAssociation* tva = NULL;
 
  // ATR-15665 for trigger: reimplementation of TrackVertexAssociationTool::buildTrackVertexAssociation_custom
  if(inTrigger){ 
      if(tracksForTJVA.size()==0){ATH_MSG_DEBUG("No tracks survived selection"); return ElementLink<xAOD::VertexContainer>();}
      else ATH_MSG_DEBUG("Selected tracks with size " << tracksForTJVA.size());

      ATH_MSG_DEBUG("Creating online TJVA"); 
      ATH_MSG_DEBUG("Building online track-vertex association trk size="<< tracksForTJVA.size()
                      << "  vtx size="<< vertices.size());
      m_matchedVertexOnline.resize(tracksForTJVA.size(), 0 );

      for (size_t iTrack = 0; iTrack < tracksForTJVA.size(); ++iTrack)
      {
          const xAOD::TrackParticle* track = tracksForTJVA.at(iTrack);
          
          // Apply track transverse distance cut
          const float transverseDistance = track->d0();//perigeeParameters().parameters()[Trk::d0];
          if (transverseDistance > m_transDistMax) continue;
                 
          // Get track longitudinal distance offset
          const float longitudinalDistance = track->z0()+track->vz();
          
          double sinTheta = std::sin(track->theta());
          
          // For each track, find the vertex with highest sum pt^2 within z0 cut
          size_t matchedIndex = 0;
          bool foundMatch = false;
          for (size_t iVertex = 0; iVertex < vertices.size(); ++iVertex)
          {
              const xAOD::Vertex* vertex = vertices.at(iVertex);
              
              double deltaz = longitudinalDistance - vertex->z();
              
              // Check longitudinal distance between track and vertex
              if ( fabs(deltaz)  > m_longDistMax)
                  continue;
              
              // Check z0*sinThetha between track and vertex
              if (fabs(deltaz*sinTheta) > m_maxZ0SinTheta)
                  continue;
              
              // If it passed the cuts, then this is the vertex we want
              // This does make the assumption that the container is sorted in sum pT^2 order
              foundMatch = true;
              matchedIndex = iVertex;
              break;
          }
          
          // If we matched a vertex, then associate that vertex to the track
          if (foundMatch)
              m_matchedVertexOnline[ iTrack ] = vertices.at(matchedIndex);
      }
  } else {
   
    if (evtStore()->retrieve(tva,m_trackVertexAssocName).isFailure()) {
       ATH_MSG_ERROR("Could not retrieve the TrackVertexAssociation from evtStore: " << m_trackVertexAssocName);
       return ElementLink<xAOD::VertexContainer>();
    }
  }
 

  // Get the highest JVF vertex and store maxJVF for later use
  // Note: the official JetMomentTools/JetVertexFractionTool doesn't provide any possibility to access the JVF value, but just the vertex.
  maxJVF=-100.;
  const xAOD::Vertex* max_vert = nullptr;
  size_t iVertex = 0;
  size_t maxIndex = 0;
  for (const xAOD::Vertex* vert : vertices) {
    float jvf = 0;
    if(!inTrigger) jvf = getJetVertexFraction(vert,tracksForTJVA,tva);
    else jvf = getJetVertexFraction(vert,tracksForTJVA);
    if (jvf > maxJVF) {
      maxJVF = jvf;
      max_vert = vert;
      maxIndex = iVertex;
    }
    ++iVertex;
  }

  ATH_MSG_DEBUG("TJVA vtx found at z: " << max_vert->z());
  ATH_MSG_DEBUG("highest pt vtx found at z: " << vertices.at(0)->z());
    
  return ElementLink<xAOD::VertexContainer> (vertices, maxIndex);
}

// reimplementation of JetVertexFractionTool::getJetVertexFraction(const xAOD::Vertex* vertex, const std::vector<const xAOD::TrackParticle*>& tracks, const jet::TrackVertexAssociation* tva) const
// avoid to call this specific tool only for this easy purpose
// see https://svnweb.cern.ch/trac/atlasoff/browser/Reconstruction/Jet/JetMomentTools/trunk/Root/JetVertexFractionTool.cxx
float TauVertexFinder::getJetVertexFraction(const xAOD::Vertex* vertex, const std::vector<const xAOD::TrackParticle*>& tracks, const jet::TrackVertexAssociation* tva) const
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
// for online ATR-15665: reimplementation needed for online because the tva doesn't work. The size of the track collection from TE is not the same as the max track index
float TauVertexFinder::getJetVertexFraction(const xAOD::Vertex* vertex, const std::vector<const xAOD::TrackParticle*>& tracks) const
{
  float sumTrackPV = 0;
  float sumTrackAll = 0;
  for (size_t iTrack = 0; iTrack < tracks.size(); ++iTrack)
    {
      const xAOD::Vertex* ptvtx = m_matchedVertexOnline[iTrack];
      if (ptvtx != nullptr) {  // C++11 feature
        if (ptvtx->index() == vertex->index()) sumTrackPV += tracks.at(iTrack)->pt();
      }
      sumTrackAll += tracks.at(iTrack)->pt();

    }
  return sumTrackAll!=0 ? sumTrackPV/sumTrackAll : 0;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Helpers
template <class T>
bool TauVertexFinder::openContainer(T* &container, std::string containerName, bool printFATAL) {
  StatusCode sc = evtStore()->retrieve(container, containerName);
  if (sc.isFailure() || !container) {
    if (printFATAL) ATH_MSG_FATAL("Container (" << containerName << ") not found in StoreGate");
    return 0;
  }
  return container;
}

template <class T>
bool TauVertexFinder::retrieveTool(T & tool) {
  if (tool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool " << tool);
    return false;
  } else {
    ATH_MSG_VERBOSE("Retrieved tool " << tool);
  }
  return true;
}
#endif
