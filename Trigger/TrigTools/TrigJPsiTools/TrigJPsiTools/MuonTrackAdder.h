// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// file name: MuonTrackAdder.h
// description: Add muon tracks to member vector
// author: Peter Wijeratne (paw@hep.ucl.ac.uk)

#ifndef MUONTRACKADDER_H
#define MUONTRACKADDER_H

#include "TrigJPsiTools/MuonTrack.h"

#include <vector>

class MuonTrackAdder{

 public:

  MuonTrackAdder() { }
  virtual ~MuonTrackAdder() { }

  virtual bool muonAdder( Wrapper::MuonTrack* t ) {
    if( t ){
	m_tracks.push_back(t); 
	return true; 
    }
    else return false;
  }

  const std::vector< Wrapper::MuonTrack* >& tracks() const { return m_tracks; }

  virtual void clear() { for(unsigned i=0; i--; ) delete m_tracks[i]; m_tracks.clear(); } 

protected:
  
  std::vector<Wrapper::MuonTrack*> m_tracks;

};

#endif
