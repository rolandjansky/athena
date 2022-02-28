/* emacs: this is -*- c++ -*- */
/**
 **     @file    TIDAVertexBuilder.h
 **
 **     @author  emil haines
 **     @date    Wed 18 Aug 2021
 **
 **     Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 **/

#ifndef  TIDAVERTEXBUILDER_H
#define  TIDAVERTEXBUILDER_H

#include "TrigInDetAnalysis/TIDAVertex.h"
#include "TrigInDetAnalysis/TrackTrigObject.h"
#include "TrigInDetAnalysis/TrackFilter.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"


class TIDAVertexBuilder {

public:

    // useful typedef
    typedef std::pair< std::vector<TIDA::Vertex>, std::vector<TrackTrigObject> > VerticesTracksPair_t;

    // default constructor
    TIDAVertexBuilder() { }

    // default destructor
    ~TIDAVertexBuilder() { }

    // select() methods convert containers of xAOD::Vertex into vector of TIDA::Vertex
    // select() methods passed a TrackFilter store track ids in TrackTrigObject alongside vertices in a pair
    // If require_tracks=true, only vertices with at least one track are kept

    static std::vector<TIDA::Vertex> select( const xAOD::VertexContainer* v, 
                                             bool require_tracks=true );

    static VerticesTracksPair_t select( const xAOD::VertexContainer* v,
                                        TrackFilter* filter,
                                        bool require_tracks=true ); 

    static VerticesTracksPair_t select( xAOD::VertexContainer::const_iterator itr,
                                        xAOD::VertexContainer::const_iterator enditr,
                                        TrackFilter* filter,
                                        bool require_tracks=true );
 
private:

    // internal functions for select() methods
    static TIDA::Vertex selectVertex( const xAOD::Vertex* v );

    static TrackTrigObject selectTracks( const xAOD::Vertex* v, TrackFilter* filter );

}; // class TIDAVertexBuilder

#endif // TIDAVERTEXBUILDER_H