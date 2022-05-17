/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#include "FlavorTagDiscriminants/TrackTruthDecoratorAlg.h"

#include "StoreGate/WriteDecorHandle.h"
#include "StoreGate/ReadDecorHandle.h"

#include "InDetTrackSystematicsTools/InDetTrackTruthOriginDefs.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthVertexContainer.h"


namespace FlavorTagDiscriminants {

  TrackTruthDecoratorAlg::TrackTruthDecoratorAlg(
    const std::string& name, ISvcLocator* loc )
    : AthReentrantAlgorithm(name, loc) {}

  StatusCode TrackTruthDecoratorAlg::initialize() {
    ATH_MSG_INFO( "Inizializing " << name() << "... " );

    // Initialize Container keys
    ATH_MSG_DEBUG( "Inizializing containers:"            );
    ATH_MSG_DEBUG( "    ** " << m_TrackContainerKey      );
    ATH_MSG_DEBUG( "    ** " << m_TruthEventsKey      );

    ATH_CHECK( m_TrackContainerKey.initialize() );
    ATH_CHECK( m_TruthEventsKey.initialize() );

    // Prepare decorators
    m_dec_origin_label = m_TrackContainerKey.key() + "." + m_dec_origin_label.key();
    m_dec_vertex_index = m_TrackContainerKey.key() + "." + m_dec_vertex_index.key();

    // Initialize decorators
    ATH_MSG_DEBUG( "Inizializing decorators:"  );
    ATH_MSG_DEBUG( "    ** " << m_dec_origin_label );
    ATH_MSG_DEBUG( "    ** " << m_dec_vertex_index );

    CHECK( m_dec_origin_label.initialize() );
    CHECK( m_dec_vertex_index.initialize() );
    
    // Retrieve tools
    ATH_CHECK( m_trackTruthOriginTool.retrieve() );

    return StatusCode::SUCCESS;
  }

  StatusCode TrackTruthDecoratorAlg::execute(const EventContext& ctx) const {
    ATH_MSG_DEBUG( "Executing " << name() << "... " );

    using TPC = xAOD::TrackParticleContainer;

    // read collections
    SG::ReadHandle<TPC> tracks(m_TrackContainerKey,ctx);
    CHECK( tracks.isValid() );
    ATH_MSG_DEBUG( "Retrieved " << tracks->size() << " tracks..." );
    SG::ReadHandle<xAOD::TruthEventContainer> truth_events(m_TruthEventsKey,ctx);
    CHECK( truth_events.isValid() );
    ATH_MSG_DEBUG( "Retrieved " << truth_events->size() << " truth events..." );
    
    // instantiate decorators
    SG::WriteDecorHandle<TPC, int> dec_origin_label(m_dec_origin_label, ctx);
    SG::WriteDecorHandle<TPC, int> dec_vertex_index(m_dec_vertex_index, ctx);

    // get the truth primary vertex
    const xAOD::TruthVertex* truth_PV = truth_events->at(0)->truthVertex(0);

    // sort the tracks by pt to ensure the vertex clustering is deterministic
    std::vector<const xAOD::TrackParticle*> sorted_tracks;
    for (const auto track : *tracks) { sorted_tracks.push_back(track); }
    std::sort(sorted_tracks.begin(), sorted_tracks.end(), sort_tracks);

    // first loop - decorate origin label, just store truth vertex for now
    auto trk_truth_vertex = std::vector<const xAOD::TruthVertex*>();
    for ( const auto& track : sorted_tracks ) {

      // store the truth origin of the track
      int trackTruthOrigin = m_trackTruthOriginTool->getTrackOrigin(track);

      // get exclusive track origin as defined
      int trackTruthLabel = InDet::ExclusiveOrigin::getExclusiveOrigin(trackTruthOrigin);
      dec_origin_label(*track) = trackTruthLabel;
      
      // get the truth vertex of the track and store for now
      auto truth_vertex = get_truth_vertex(track);
      trk_truth_vertex.push_back(truth_vertex);
    }

    // decorate tracks with truth vertex info
    auto seen_vertices = std::vector<const xAOD::TruthVertex*>();
    for ( size_t i = 0; i != trk_truth_vertex.size(); i++) {
      auto this_vert  = trk_truth_vertex.at(i);
      auto this_track = sorted_tracks.at(i);

      // do we have a vertex for this track?
      if ( !this_vert ) {
        dec_vertex_index(*this_track) = -2;
        continue;
      }    

      // track from PV
      if ( get_distance(this_vert, truth_PV) < m_truthVertexMergeDistance ) {
        dec_vertex_index(*this_track) = 0;
        continue;
      }

      // have we already seen this vertex?
      bool new_vertex = true;
      for ( size_t j = 0; j != seen_vertices.size(); j++) {
        float dr = get_distance(seen_vertices.at(j), this_vert);
        if ( dr < m_truthVertexMergeDistance ) {
          // a vertex is nearby, reuse it
          new_vertex = false;
          dec_vertex_index(*this_track) = j+1;
          break;
        }
      }

      // this vertex is far enough away from others to be considered unique
      if ( new_vertex ) {
        dec_vertex_index(*this_track) = seen_vertices.size()+1;
        seen_vertices.push_back(this_vert);
      }
    }
    return StatusCode::SUCCESS;
  }

  bool TrackTruthDecoratorAlg::sort_tracks(const xAOD::TrackParticle* track_A, 
                                           const xAOD::TrackParticle* track_B) {
    return track_A->pt() < track_B->pt();
  }

  const xAOD::TruthVertex* TrackTruthDecoratorAlg::get_truth_vertex(
    const xAOD::TrackParticle* track) const {
    
    const xAOD::TruthParticle* truth = m_trackTruthOriginTool->getTruth(track);
    if ( not truth ) { return nullptr; }

    // no vertex
    const xAOD::TruthVertex* truth_vertex = truth->prodVtx();
    if ( not truth_vertex or truth_vertex->perp() > 440.0 ) { 
      return nullptr;
    }

    return truth_vertex;
  }

  const xAOD::TruthVertex* TrackTruthDecoratorAlg::get_nearest_vertex(
    const xAOD::TruthVertex* search_vertex,
    std::vector<const xAOD::TruthVertex*> vertices) const {

    if ( !search_vertex ) { return nullptr; }

    float min_dr = 1e5;
    int min_dr_idx = -1;
    int index = -1;

    // find closest vertex
    for ( auto vertex : vertices ) {
      index++;
      if ( !vertex or search_vertex == vertex ) { continue; }
      float dr = get_distance(vertex, search_vertex);
      if ( dr < min_dr ) {
        min_dr = dr;
        min_dr_idx = index;
      }
    }

    // return closest vertex
    if ( min_dr_idx < 0 ) { return nullptr; }
    else { return vertices.at(min_dr_idx); }
  }

  float TrackTruthDecoratorAlg::get_distance(const xAOD::TruthVertex* vertex_A, 
                                             const xAOD::TruthVertex* vertex_B) const {
    if ( !vertex_A or !vertex_B ) { return 999.0; }
    return (vertex_A->v4().Vect() - vertex_B->v4().Vect()).Mag();
  }

}


