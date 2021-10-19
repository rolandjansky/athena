/* emacs: this is -*- c++ -*- */
/**
 **     @file    TIDAVertexNew.h
 **
 **     @author  emil haines
 **     @date    Wed  28 Jul 2021
 **
**     Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef  TIDAVERTEXNEW_H
#define  TIDAVERTEXNEW_H

#include <iostream>
#include <vector>

#include "TrigInDetAnalysis/TIDAVertex.h"
#include "TrigInDetAnalysis/TrackTrigObject.h"
#include "TrigInDetAnalysis/Track.h"

namespace TIDA {

  class VertexNew : public TIDA::Vertex {

  public:

    VertexNew( const TIDA::Vertex& v ) : Vertex(v) {}
      
    VertexNew( const TIDA::Vertex& v, 
               const TrackTrigObject& tobject, 
               const std::vector<TIDA::Track*>* refTracks=0 );

    // copy constructor
    VertexNew( const TIDA::VertexNew& v,  
               const std::vector<TIDA::Track*>* refTracks=0,
               const TrackTrigObject* tobject = 0 );

    virtual ~VertexNew() {}

    void selectTracks( const std::vector<TIDA::Track*>* refTracks,
                       const TrackTrigObject* tobject = 0 );

    const std::vector<TIDA::Track*>& tracks() const { return m_tracks; }

    const std::vector<unsigned long>& ids() const { return m_ids; }

  private:

    void selectTracksInternal( const std::vector<TIDA::Track*>* refTracks );

    void addTrack(TIDA::Track* trk) { m_tracks.push_back(trk); }

    std::vector<TIDA::Track*> m_tracks;

    std::vector<unsigned long> m_ids;
      
  };

}

#endif // TIDAVERTEXNEW.H