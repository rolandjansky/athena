/* emacs: this is -*- c++ -*- */
/**
 **     @file    VertexNewMatcher.h
 **
 **     @author  emil haines
 **     @date    Thu 29 Jul 2021
 **
**     Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 **/

#ifndef  VERTEXNEWMATCHER_H
#define  VERTEXNEWMATCHER_H

#include <iostream>

#include "TrigInDetAnalysisUtils/BestMatcher.h"
#include "TrigInDetAnalysisUtils/Associator_BestMatch.h"

#include "TrigInDetAnalysis/TIDAVertex.h"


class VertexNewMatcher : public BestMatcher<TIDA::Vertex> {

public:

  VertexNewMatcher( const std::string& s, double d=0.5 ) : BestMatcher<TIDA::Vertex>( s, d*d ) { }  

  ~VertexNewMatcher() { }
  
  virtual double distance( const TIDA::Vertex* v0, const TIDA::Vertex* v1 ) const {

    // vectors of pointers to tracks belonging to ref and test vertices
    const std::vector<TIDA::Track*>& refTracks= v0->tracks();
    const std::vector<TIDA::Track*>& testTracks = v1->tracks();

    // match offline to trigger tracks using delta R
    Associator_DeltaRMatcher track_matcher( "track_matcher", 0.05 );
    track_matcher.match( refTracks, testTracks );

    double n_matches = (double)track_matcher.size();
    double eff = n_matches/refTracks.size();

    // 1-eff as BestMatcher uses lower than threshold, squared as BestMatcer takes d*d
    return (1-eff)*(1-eff);
  };
  
};

inline std::ostream& operator<<( std::ostream& s, const VertexNewMatcher& /* _v */ ) { 
  return s;
}

#endif  // VERTEXNEWMATCHER_H



