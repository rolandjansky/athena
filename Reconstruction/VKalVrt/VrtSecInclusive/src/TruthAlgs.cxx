/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "VrtSecInclusive/VrtSecInclusive.h"
#include "VrtSecInclusive/NtupleVars.h"

#include <iostream>
#include <set>
#include <tuple>

#include "TH1D.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TROOT.h"
//-------------------------------------------------

using namespace std;

namespace VKalVrtAthena {
  
  //____________________________________________________________________________________________________
  const xAOD::TruthParticle*  VrtSecInclusive::getTrkGenParticle ( const xAOD::TrackParticle *trkPart ) const 
  {
    typedef ElementLink< xAOD::TruthParticleContainer > Link_t;
    static const char* NAME = "truthParticleLink";
    if(  ! trkPart->isAvailable< Link_t >( NAME ) ) {
      return 0;
    }
    const Link_t& link = trkPart->auxdata< Link_t >(  NAME );
    if(  ! link.isValid() ) {
      return 0;
    }
    return *link;
  }
  
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::categorizeVertexTruthTopology(  xAOD::Vertex *vertex )
  {
    
    enum vertexCatogory_tracks {
      allTruthAssociated,
      hasFakeTracks,
      allFakeTracks
    };
    
    enum vertexCategory_vertex {
      uniqueTruthVertex,
      multipleTruthVertices,
      noTruthVertex
    };
    
    
    // std::multiset allows to have multiplicity of the element
    multiset<const xAOD::TruthVertex*> truth_vertices;
    
    // std::multiset doesn't allow to have multiplicity of the element
    set<const xAOD::TruthVertex*>      truth_vertices_types;
    
    vector<const xAOD::TrackParticle*> reco_tracks;   // associated with truth and has prodVtx
    vector<const xAOD::TrackParticle*> orphan_tracks; // associated with truth, but does not have prodVtx
    vector<const xAOD::TrackParticle*> fake_tracks;   // no association with truth ( fake )
    
    // loop over tracks
    ATH_MSG_VERBOSE( "categorizeVertexTruthTopology(): loop over tracks" );
    for(  size_t itrk=0; itrk<vertex->nTrackParticles(); itrk++ ) {
      const auto *trk = vertex->trackParticle( itrk );
	
      ATH_MSG_VERBOSE( "categorizeVertexTruthTopology(): track loop itrk = " << itrk );
      typedef ElementLink<xAOD::TruthParticleContainer> truthLink;
      const truthLink& link = trk->auxdataConst< truthLink >( "truthParticleLink" );
      
      if (  ! link ) {
	fake_tracks.emplace_back(  trk );
	continue;
      }
      
      const xAOD::TruthParticle *truth = *link;
      if(  ! truth->hasProdVtx() ) {
	orphan_tracks.emplace_back(  trk );
	continue;
      }
      
      reco_tracks.emplace_back(  trk );
      
      truth_vertices_types .insert(  truth->prodVtx() );
      truth_vertices       .insert(  truth->prodVtx() );
	
    }
    
    
    // Add truth track pattern to the reco vertex
    ATH_MSG_VERBOSE( "categorizeVertexTruthTopology(): Add truth track pattern to the reco vertex" );
    if(  reco_tracks.size() == vertex->nTrackParticles() ) {
      vertex->auxdata<char>( "truth_trk_pattern" ) = allTruthAssociated;
    } else if(  fake_tracks.size() == vertex->nTrackParticles() ) {
      vertex->auxdata<char>( "truth_trk_pattern" ) = allFakeTracks;
    } else {
      vertex->auxdata<char>( "truth_trk_pattern" ) = hasFakeTracks;
    }
    
    
    // Histogramming - counting the number of appearing truth vertices connected
    ATH_MSG_VERBOSE( "categorizeVertexTruthTopology(): Histogramming - counting the number of appearing truth vertices connected" );
    vector< tuple<const xAOD::TruthVertex*, size_t> > truth_vertex_histogram;
    for(  const auto *v : truth_vertices_types ) {
      size_t count = truth_vertices.count(  v );
      truth_vertex_histogram.emplace_back(  tuple<const xAOD::TruthVertex*, size_t>( v, count ) );
    }
    
    // Determine the truth vertex associated to this vertex by majority decision
    ATH_MSG_VERBOSE( "categorizeVertexTruthTopology(): Determine the truth vertex associated to this vertex by majority decision" );
    tuple<const xAOD::TruthVertex*, size_t> tmp_tuple(  nullptr, 0 );
    for(  const auto& t : truth_vertex_histogram ) {
      const size_t& size_tmp  = get<1>( tmp_tuple );
      const size_t& size_this = get<1>( t );
      if(  size_tmp < size_this ) tmp_tuple = t;
    }
    
    // Add truth track pattern to the reco vertex
    ATH_MSG_VERBOSE( "categorizeVertexTruthTopology(): Add truth track pattern to the reco vertex" );
    char truth_vtx_pattern = 0;
    if(  truth_vertices_types.size() == 0 ) {
      truth_vtx_pattern = noTruthVertex;
    } else if(  truth_vertices_types.size() == 1 ) {
      truth_vtx_pattern = uniqueTruthVertex;
    } else {
      truth_vtx_pattern = multipleTruthVertices;
    }
    vertex->auxdata<char>( "truth_vtx_pattern" ) = truth_vtx_pattern;
    
    
    ElementLink<xAOD::TruthVertexContainer> vtx_link;
    if( noTruthVertex != truth_vtx_pattern ) {
        
      // Retrieve the truth vertex container for element link
      ATH_MSG_VERBOSE( "categorizeVertexTruthTopology(): Retrieve the truth vertex container for element link" );
      const xAOD::TruthVertexContainer* truthVertexContainer (  nullptr );
      ATH_CHECK(  evtStore()->retrieve( truthVertexContainer, "TruthVertices") );
    
      // create the element link
      ATH_MSG_VERBOSE( "categorizeVertexTruthTopology(): create the element link" );
      const auto *theVertex = get<0>( tmp_tuple );
      if(  theVertex ) {
          // Add the truth vertex element link to the reco vertex
          vtx_link.toIndexedElement(*truthVertexContainer,theVertex->index());
          ATH_MSG_VERBOSE( "categorizeVertexTruthTopology(): Add the truth vertex element link to the reco vertex" );
      }
    } 
    // [JDC] a ElementLink decorator should be filled every event
    // although using a null link
    vertex->auxdata<ElementLink<xAOD::TruthVertexContainer> >( "truth_vtx_link" ) = vtx_link;
            
    return StatusCode::SUCCESS;
  }
  
} // end of namespace VKalVrtAthena



