/**
 **     @file    TIDAVertexBuilder.cxx
 **
 **     @author  emil haines
 **     @date    Wed 18 Aug 2021
 **
 **     Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 **/

#include "TrigInDetAnalysisUtils/TIDAVertexBuilder.h"


std::vector<TIDA::Vertex> TIDAVertexBuilder::select( 
    const xAOD::VertexContainer* xAODVertices,
    const std::vector<TIDA::Track*>* trackCollection, 
    bool require_tracks ) {

  std::vector<TIDA::Vertex> vertices;
  
  xAOD::VertexContainer::const_iterator vtxitr = xAODVertices->begin();
  
  for ( ; vtxitr != xAODVertices->end(); ++vtxitr ) {
  
      if ( require_tracks && (*vtxitr)->nTrackParticles()==0 ) continue;
  
      if ( (*vtxitr)->vertexType()!=0 ) { 
          TIDA::Vertex vertex = selectVertex( *vtxitr );
          
          if( trackCollection ) {
            std::vector< unsigned long > trackIds;

            const std::vector< ElementLink< xAOD::TrackParticleContainer > >& xAODtracks = (*vtxitr)->trackParticleLinks();

            for( const auto& track : xAODtracks ) {
              unsigned long id = getTrackId( *track );
              trackIds.push_back( id );
            }

            vertex.selectTracks( *trackCollection, trackIds );
          }

          vertices.push_back( vertex );
      }
  }

  return vertices;
}


std::vector<TIDA::Vertex> TIDAVertexBuilder::select( 
    xAOD::VertexContainer::const_iterator itr,
    xAOD::VertexContainer::const_iterator enditr,
    const std::vector<TIDA::Track*>* trackCollection,
    bool require_tracks ) {

  std::vector<TIDA::Vertex> vertices;

  xAOD::VertexContainer::const_iterator vtxitr = itr;
  
  for ( ; vtxitr!=enditr; ++vtxitr ) {
  
      if ( require_tracks && (*vtxitr)->nTrackParticles()==0 ) continue; 
  
      if ( (*vtxitr)->vertexType()!=0 ) {

          TIDA::Vertex vertex = selectVertex( *vtxitr );
          
          if( trackCollection ) {
            std::vector< unsigned long > trackIds;

            const std::vector< ElementLink< xAOD::TrackParticleContainer > >& xAODtracks = (*vtxitr)->trackParticleLinks();

            for( const auto& track : xAODtracks ) {
              unsigned long id = getTrackId( *track );
              trackIds.push_back( id );
            }

            vertex.selectTracks( *trackCollection, trackIds );
          }

          vertices.push_back( vertex );
      }
  }
    
  return vertices;
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


unsigned long TIDAVertexBuilder::getTrackId( const xAOD::TrackParticle* track ) {
  unsigned long id = (unsigned long)track;
  return id;
}
