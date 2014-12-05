/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetVertexFractionTool.cxx

#include "JetMomentTools/JetVertexFractionTool.h"

//**********************************************************************

JetVertexFractionTool::JetVertexFractionTool(const std::string& name)
: JetModifierBase(name)
, m_verticesName("")
, m_assocTracksName("")
, m_tvaName(""),
  m_htsel("") {
  declareProperty("VertexContainer", m_verticesName);
  declareProperty("AssociatedTracks", m_assocTracksName);
  declareProperty("TrackVertexAssociation", m_tvaName);
  declareProperty("TrackSelector", m_htsel);
  declareProperty("JVFName", m_jvfname ="JVF");
}

//**********************************************************************

StatusCode JetVertexFractionTool::initialize() {
  ATH_MSG_INFO("Initializing JetVertexFractionTool " << name());
  if ( m_htsel.empty() ) {
    ATH_MSG_INFO("  No track selector.");
  } else {
    ATH_MSG_INFO("  Track selector: " << m_htsel->name());
  }
  ATH_MSG_INFO("  Attribute name: " << m_jvfname);
  return StatusCode::SUCCESS;
}

//**********************************************************************

int JetVertexFractionTool::modifyJet(xAOD::Jet& jet) const {

  // Get the vertices container
  const xAOD::VertexContainer* vertices = NULL;
  if ( evtStore()->retrieve(vertices,m_verticesName).isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve the VertexContainer from evtStore: " << m_verticesName);
    return 1;
  }
  ATH_MSG_DEBUG("Successfully retrieved VertexContainer from evtStore: " << m_verticesName);
    
  // Get the tracks associated to the jet
  // Note that there may be no tracks - this is both normal and an error case
  // In this case, just fill a vector with zero and don't set the highest vtx moment
  std::vector<const xAOD::TrackParticle*> tracks;
  if ( ! jet.getAssociatedObjects(m_assocTracksName, tracks) ) {
    ATH_MSG_WARNING("Associated tracks not found.");
  }

  // Get the TVA object
  const jet::TrackVertexAssociation* tva = NULL;
  if (evtStore()->retrieve(tva,m_tvaName).isFailure()) {
    ATH_MSG_ERROR("Could not retrieve the TrackVertexAssociation from evtStore: " << m_tvaName);
    return 3;
  }
  ATH_MSG_DEBUG("Successfully retrieved TrackVertexAssociation from evtStore: " << m_tvaName);

  // Get and set the JVF vector
  const std::vector<float> jvf = getJetVertexFraction(vertices,tracks,tva);
  jet.setAttribute(m_jvfname, jvf);

  // Get and set the highest JVF vertex
  jet.setAttribute("Highest" + m_jvfname + "Vtx",getMaxJetVertexFraction(vertices,jvf));
    
  // Done
  return 0;
}

//**********************************************************************

std::vector<float>
JetVertexFractionTool::getEmptyJetVertexFraction(const xAOD::VertexContainer* vertices) const {
  std::vector<float> jvf;
  jvf.resize(vertices->size());
  for (size_t iVertex = 0; iVertex < vertices->size(); ++iVertex) jvf.at(iVertex) = 0;
  return jvf;
}

//**********************************************************************

const std::vector<float> JetVertexFractionTool::
getJetVertexFraction(const xAOD::VertexContainer* vertices,
                     const std::vector<const xAOD::TrackParticle*>& tracks,
                     const jet::TrackVertexAssociation* tva) const {
  std::vector<float> jvf = getEmptyJetVertexFraction(vertices);
  for ( size_t iVertex = 0; iVertex < vertices->size(); ++iVertex )
    jvf.at(iVertex) = getJetVertexFraction(vertices->at(iVertex),tracks,tva);
  return jvf;
}


//**********************************************************************

float JetVertexFractionTool::
getJetVertexFraction(const xAOD::Vertex* vertex,
                     const std::vector<const xAOD::TrackParticle*>& tracks,
                     const jet::TrackVertexAssociation* tva) const {
  float sumTrackPV = 0;
  float sumTrackAll = 0;
  bool notsel = m_htsel.empty();
  unsigned int nkeep = 0;
  unsigned int nskip = 0;
  for ( size_t itrk = 0; itrk < tracks.size(); ++itrk ) {
    const xAOD::TrackParticle* ptrk = tracks.at(itrk);
    if ( notsel || m_htsel->keep(*ptrk) ) {
      const xAOD::Vertex* ptvtx = tva->associatedVertex(ptrk);
      if ( ptvtx != nullptr ) {
        if ( ptvtx->index() == vertex->index() ) sumTrackPV += ptrk->pt();
      }
      sumTrackAll += ptrk->pt();
      ++nkeep;
    } else {
      ++nskip;
    }
  }
  double jvf = sumTrackAll>0 ? sumTrackPV/sumTrackAll : -1;
  ATH_MSG_VERBOSE("JetVertexFractionTool " << name()
                  << ": nsel=" << nkeep
                  << ", nrej=" << nskip
                  << ", " << m_jvfname << "=" << jvf);
  return jvf;
}


//**********************************************************************

ElementLink<xAOD::VertexContainer> JetVertexFractionTool::
getMaxJetVertexFraction(const xAOD::VertexContainer* vertices,
                        const std::vector<float>& jvf) const {
  size_t maxIndex = 0;
  float maxVal = -100;
  for ( size_t iVertex = 0; iVertex < jvf.size(); ++iVertex ) {
    if ( jvf.at(iVertex) > maxVal ) {
      maxIndex = iVertex;
      maxVal = jvf.at(iVertex);
    }
  }
  ElementLink<xAOD::VertexContainer> link =
    ElementLink<xAOD::VertexContainer>(*vertices,vertices->at(maxIndex)->index());
  return link;
}

//**********************************************************************
