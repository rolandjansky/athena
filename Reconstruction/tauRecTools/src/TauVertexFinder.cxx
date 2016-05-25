/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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
  m_maxJVF(-100.),
  m_TrackSelectionToolForTJVA(""),
  m_assocTracksName(""),
  m_trackVertexAssocName("")
{
  declareProperty("UseTJVA", m_useTJVA=true);
  declareProperty("PrimaryVertexContainer", m_inputPrimaryVertexContainerName = "PrimaryVertices");
  declareProperty("AssociatedTracks",m_assocTracksName);
  declareProperty("TrackVertexAssociation",m_trackVertexAssocName);
  declareProperty("InDetTrackSelectionToolForTJVA",m_TrackSelectionToolForTJVA);
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

  ElementLink<xAOD::VertexContainer> newPrimaryVertexLink = getPV_TJVA(pTau, *vxContainer );
  if (newPrimaryVertexLink.isValid()) {
    // set new primary vertex
    // will overwrite default one which was set above
    pTau.setVertexLink(newPrimaryVertexLink);
    // save highest JVF value
    pTau.setDetail(xAOD::TauJetParameters::TauJetVtxFraction,static_cast<float>(m_maxJVF));
    ATH_MSG_DEBUG("TJVA vertex found and set");
  }
  else {
    ATH_MSG_DEBUG("couldn't find new PV for TJVA");
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
ElementLink<xAOD::VertexContainer> TauVertexFinder::getPV_TJVA(const xAOD::TauJet& pTau, const xAOD::VertexContainer& vertices)
{
  const xAOD::Jet* pJetSeed = (*pTau.jetLink());
  std::vector<const xAOD::TrackParticle*> tracksForTJVA;
  const double dDeltaRMax(0.2);

  // the implementation follows closely the example given in modifyJet(...) in https://svnweb.cern.ch/trac/atlasoff/browser/Reconstruction/Jet/JetMomentTools/trunk/Root/JetVertexFractionTool.cxx#15

  // Get the tracks associated to the jet
  std::vector<const xAOD::TrackParticle*> assocTracks;
  if (! pJetSeed->getAssociatedObjects(m_assocTracksName, assocTracks)) {
    ATH_MSG_ERROR("Could not retrieve the AssociatedObjects named \""<< m_assocTracksName <<"\" from jet");
    return ElementLink<xAOD::VertexContainer>();
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
  if (evtStore()->retrieve(tva,m_trackVertexAssocName).isFailure()) {
    ATH_MSG_ERROR("Could not retrieve the TrackVertexAssociation from evtStore: " << m_trackVertexAssocName);
    return ElementLink<xAOD::VertexContainer>();
  }

  // Calculate Jet Vertex Fraction
  std::vector<float> jvf;
  jvf.resize(vertices.size());
  for (size_t iVertex = 0; iVertex < vertices.size(); ++iVertex) {
    jvf.at(iVertex) = getJetVertexFraction(vertices.at(iVertex),tracksForTJVA,tva);
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
  ElementLink<xAOD::VertexContainer> vtxlink = ElementLink<xAOD::VertexContainer>(vertices,vertices.at(maxIndex)->index());

  return vtxlink;
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
