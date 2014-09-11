/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetMomentTools/JetVertexFractionTool.h"

JetVertexFractionTool::JetVertexFractionTool(const std::string& name)
: JetModifierBase(name)
, m_verticesName("")
, m_assocTracksName("")
, m_tvaName("")
{
    declareProperty("VertexContainer",m_verticesName);
    declareProperty("AssociatedTracks",m_assocTracksName);
    declareProperty("TrackVertexAssociation",m_tvaName);
}

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
  jet.setAttribute("JVF",jvf);

  // Get and set the highest JVF vertex
  jet.setAttribute("HighestJVFVtx",getMaxJetVertexFraction(vertices,jvf));
    
  // Done
  return 0;
}



std::vector<float> JetVertexFractionTool::getEmptyJetVertexFraction(const xAOD::VertexContainer* vertices) const
{
    std::vector<float> jvf;
    jvf.resize(vertices->size());
    for (size_t iVertex = 0; iVertex < vertices->size(); ++iVertex)
        jvf.at(iVertex) = 0;

    return jvf;
}

const std::vector<float> JetVertexFractionTool::getJetVertexFraction(const xAOD::VertexContainer* vertices, const std::vector<const xAOD::TrackParticle*>& tracks, const jet::TrackVertexAssociation* tva) const
{
    std::vector<float> jvf = getEmptyJetVertexFraction(vertices);

    for (size_t iVertex = 0; iVertex < vertices->size(); ++iVertex)
        jvf.at(iVertex) = getJetVertexFraction(vertices->at(iVertex),tracks,tva);

    return jvf;
}


float JetVertexFractionTool::getJetVertexFraction(const xAOD::Vertex* vertex, const std::vector<const xAOD::TrackParticle*>& tracks, const jet::TrackVertexAssociation* tva) const
{
    float sumTrackPV = 0;
    float sumTrackAll = 0;

    for (size_t iTrack = 0; iTrack < tracks.size(); ++iTrack) {
      const xAOD::TrackParticle* track = tracks.at(iTrack);
      const xAOD::Vertex* ptvtx = tva->associatedVertex(track);
      if ( ptvtx != nullptr ) {
        if ( ptvtx->index() == vertex->index() ) sumTrackPV += track->pt();
      }
      sumTrackAll += track->pt();
    }

    return sumTrackAll>0 ? sumTrackPV/sumTrackAll : -1;
}


ElementLink<xAOD::VertexContainer> JetVertexFractionTool::getMaxJetVertexFraction(const xAOD::VertexContainer* vertices, const std::vector<float>& jvf) const
{
    size_t maxIndex = 0;
    float maxVal = -100;

    for (size_t iVertex = 0; iVertex < jvf.size(); ++iVertex)
        if (jvf.at(iVertex) > maxVal)
        {
            maxIndex = iVertex;
            maxVal = jvf.at(iVertex);
        }

    ElementLink<xAOD::VertexContainer> link = ElementLink<xAOD::VertexContainer>(*vertices,vertices->at(maxIndex)->index());

    return link;
}



