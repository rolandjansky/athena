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
	mtracks.push_back(t); 
	return true; 
    }
    else return false;
  }

  const std::vector< Wrapper::MuonTrack* >& tracks() const { return mtracks; }

  virtual void clear() { for(unsigned i=0; i--; ) delete mtracks[i]; mtracks.clear(); } 

protected:
  
  std::vector<Wrapper::MuonTrack*> mtracks;

};

#endif
