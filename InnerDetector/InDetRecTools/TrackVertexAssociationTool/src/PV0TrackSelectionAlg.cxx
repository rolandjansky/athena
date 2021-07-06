/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PV0TrackSelectionAlg.h"

PV0TrackSelectionAlg::PV0TrackSelectionAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthReentrantAlgorithm( name, pSvcLocator ){}

StatusCode PV0TrackSelectionAlg::initialize(){
  ATH_MSG_INFO ("Initializing " << name() << "...");

  ATH_CHECK(m_inTrackKey.initialize());
  ATH_CHECK(m_vertexKey.initialize());
  ATH_CHECK(m_outTrackKey.initialize());

  ATH_CHECK(m_decorDeps.initialize(m_inTrackKey, m_outTrackKey));

  ATH_CHECK(m_TVATool.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode PV0TrackSelectionAlg::finalize(){
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode PV0TrackSelectionAlg::execute(const EventContext& ctx) const{  

  ATH_MSG_DEBUG ("Executing " << name() << "...");

  SG::ReadHandle<xAOD::TrackParticleContainer> inTracks(m_inTrackKey, ctx);
  SG::ReadHandle<xAOD::VertexContainer> vertices(m_vertexKey, ctx);

  if (!inTracks.isValid() || !vertices.isValid()) {
    if (!inTracks.isValid()) ATH_MSG_ERROR("TrackParticle container not found: " << m_inTrackKey.key());
    if (!vertices.isValid()) ATH_MSG_ERROR("Vertex container not found: " << m_vertexKey.key());
    return StatusCode::FAILURE;
  }

  // Find the primary vertex
  const xAOD::Vertex* pv(nullptr);
  for(const xAOD::Vertex* vertex : *vertices){
    if(vertex->vertexType() == xAOD::VxType::PriVtx){
      pv = vertex;
      break;
    }
  }

  auto pv0tracks = std::make_unique<ConstDataVector<xAOD::TrackParticleContainer> >( SG::VIEW_ELEMENTS );

  if(pv != nullptr){  
    for(const xAOD::TrackParticle* track : *inTracks) {
      if(m_TVATool->isCompatible(*track, *pv) ) pv0tracks->push_back(track);
    }
  }
  else ATH_MSG_DEBUG("No primary vertex found. Selecting no tracks.");

  std::unique_ptr<const xAOD::TrackParticleContainer> outTracksC(pv0tracks.release()->asDataVector());
  SG::WriteHandle<xAOD::TrackParticleContainer> outTrackHandle(m_outTrackKey, ctx);

  if(outTrackHandle.put(std::move(outTracksC)) == nullptr){
    ATH_MSG_ERROR("Failed to record " << m_outTrackKey.key() << " as const xAOD::TrackParticleContainer!");
    return StatusCode::FAILURE;
  }
  ATH_CHECK(m_decorDeps.linkDecors (m_inTrackKey, ctx));

  return StatusCode::SUCCESS;
}