/**
 **     @file    TIDAVertexBuilder.cxx
 **
 **     @author  emil haines
 **     @date    Wed 18 Aug 2021
 **
 **     Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 **/

#include "TrigInDetAnalysisUtils/TIDAVertexBuilder.h"
#include "TrigInDetAnalysisUtils/Filters.h"
#include "TrigInDetAnalysisUtils/TrigTrackSelector.h"

// useful typedef
typedef std::pair< std::vector<TIDA::Vertex>, std::vector<TrackTrigObject> > VerticesTracksPair_t;


std::vector<TIDA::Vertex> TIDAVertexBuilder::select( 
    const xAOD::VertexContainer* v, 
    bool require_tracks ) {

  std::vector<TIDA::Vertex> vertices;
  
  xAOD::VertexContainer::const_iterator vtxitr = v->begin();
  
  for ( ; vtxitr != v->end(); ++vtxitr ) {
  
      if ( require_tracks && (*vtxitr)->nTrackParticles()==0 ) continue;
  
      if ( (*vtxitr)->vertexType()!=0 ) { 
          vertices.push_back( selectVertex( *vtxitr ) );
      }
  }

return vertices;
}


VerticesTracksPair_t TIDAVertexBuilder::select( 
    const xAOD::VertexContainer* v, 
    TrackFilter* filter, 
    bool require_tracks ) {

  std::vector<TIDA::Vertex> vertices;
  std::vector<TrackTrigObject> tracks;

  xAOD::VertexContainer::const_iterator vtxitr = v->begin();
  
  for ( ; vtxitr != v->end(); ++vtxitr ) {
  
      if ( require_tracks && (*vtxitr)->nTrackParticles()==0 ) continue;
      
      if ( (*vtxitr)->vertexType()!=0 ) { 
          vertices.push_back( selectVertex( *vtxitr ) );
          tracks.push_back( selectTracks( *vtxitr, filter ) );
      }
  }
  
  VerticesTracksPair_t verticesTracksPair( vertices, tracks );
  
  return verticesTracksPair;
}


VerticesTracksPair_t TIDAVertexBuilder::select( 
    xAOD::VertexContainer::const_iterator itr,
    xAOD::VertexContainer::const_iterator enditr,
    TrackFilter* filter,
    bool require_tracks ) {

  std::vector<TIDA::Vertex> vertices;
  std::vector<TrackTrigObject> tracks;
  
  xAOD::VertexContainer::const_iterator vtxitr = itr;
  
  for ( ; vtxitr!=enditr; ++vtxitr ) {
  
      if ( require_tracks && (*vtxitr)->nTrackParticles()==0 ) continue; 
  
      if ( (*vtxitr)->vertexType()!=0 ) {
          vertices.push_back( selectVertex( *vtxitr ) );
          tracks.push_back( selectTracks( *vtxitr, filter ) );
      }
  }
  
  VerticesTracksPair_t verticesTracksPair( vertices, tracks );
  
  return verticesTracksPair;

}


TIDA::Vertex TIDAVertexBuilder::selectVertex( const xAOD::Vertex* v ) {
  
  // convert xAOD::Vertex into TIDA::Vertex
  TIDA::Vertex tidav( v->x(),
                      v->y(),
                      v->z(),
                      /// variances                                                                                          
                      v->covariancePosition()(Trk::x,Trk::x),
                      v->covariancePosition()(Trk::y,Trk::y),
                      v->covariancePosition()(Trk::z,Trk::z),
                      v->nTrackParticles(),
                      /// quality                                                                                            
                      v->chiSquared(),
                      v->numberDoF() );
  
  return tidav;

}


TrackTrigObject TIDAVertexBuilder::selectTracks( const xAOD::Vertex* v, TrackFilter* filter ) {

  // get tracks associated to vertex
  const std::vector< ElementLink< xAOD::TrackParticleContainer > >& xAODtracks = v->trackParticleLinks();
  
  // convert from xAOD into TIDA::Track
  TrigTrackSelector selector( filter ); 
  selector.selectTracks( xAODtracks );
  const std::vector<TIDA::Track*>& tidatracks = selector.tracks();
  
  // Store ids of tracks belonging to vertex in TrackTrigObject
  TrackTrigObject tracks = TrackTrigObject();
  for ( auto trkitr = tidatracks.begin(); trkitr != tidatracks.end(); ++trkitr ) {
      tracks.addChild( (*trkitr)->id() );
  }
  
  return tracks;

}
