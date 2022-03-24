/* emacs: this is -*- c++ -*- */
/**
 **     @file    TIDAVertexBuilder.h
 **
 **     @author  emil haines
 **     @date    Wed 18 Aug 2021
 **
 **     Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 **/

#ifndef  TIDA_VERTEX_BUILDER_H
#define  TIDA_VERTEX_BUILDER_H

#include "TrigInDetAnalysis/TIDAVertex.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"


class TIDAVertexBuilder {

public:

    // default constructor
    TIDAVertexBuilder() { }

    // default destructor
    ~TIDAVertexBuilder() { }

    // select() methods convert containers of xAOD::Vertex into vector of TIDA::Vertex
    // If require_tracks=true, only vertices with at least one track are kept
    static std::vector<TIDA::Vertex> select( const xAOD::VertexContainer* xAODVertices,
                                             const std::vector<TIDA::Track*>* trackCollection=0,
                                             bool require_tracks=true );


    static std::vector<TIDA::Vertex> select( xAOD::VertexContainer::const_iterator itr,
                                             xAOD::VertexContainer::const_iterator enditr,
                                             const std::vector<TIDA::Track*>* trackCollection=0,
                                             bool require_tracks=true );
 
private:

    // internal methods for select() methods
    static TIDA::Vertex selectVertex( const xAOD::Vertex* v );

    static unsigned long getTrackId( const xAOD::TrackParticle* track );

}; // class TIDAVertexBuilder

#endif // TIDA_VERTEX_BUILDER_H