// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file JpsiTrack.h **/
/** @author Peter Wijeratne (paw@hep.ucl.ac.uk) **/

#ifndef TrigIDJpsiMonitoring_JpsiTrack_H
#define TrigIDJpsiMonitoring_JpsiTrack_H

#include <iostream>
#include <vector>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/TIDAVertex.h"

#include "TrigJPsiTools/MakeDiMuonsTool.h"

#include "Particle/TrackParticle.h"

namespace TIDA{

class JpsiTrack : public TIDA::Track {

 public:

  JpsiTrack(const TIDA::Track& t) : TIDA::Track(t) { }
  
  JpsiTrack(const TIDA::Track& t, const Rec::TrackParticle* m, DiMuonTool *pair, int roi) : TIDA::Track(t), m_match(m) {
    //std::cout << "JpsiTrack constructor called." << std::endl;
    //std::cout << "pair (pointer): " << pair << std::endl;
    m_pair = pair;
    m_roi = roi;
    /*std::cout << "Call probed track..." << std::endl;
    const Wrapper::MuonTrack* probeTrack = m_pair->ProbedTrack();
    std::cout << "... done. Call tagged track..." << std::endl;
    const Wrapper::MuonTrack* tagTrack = m_pair->TaggedTrack();
    std::cout << "...done." << std::endl;
    std::cout << "... end of JpsiTrack constructor." << std::endl;*/
  }

  virtual ~JpsiTrack() { }

  const Rec::TrackParticle* hasMatch() { return m_match; }

  const Wrapper::MuonTrack* getTag() { 
    //std::cout << "JpsiTrack -- getTag() -- m_pair: " << m_pair << std::endl;
    return m_pair->TaggedTrack();
  }

  double px() const { return ( pT() * cos( phi() ) ); }
  double py() const { return ( pT() * sin( phi() ) ); }
  double pz() const { return ( pT() * cos( 2. * atan(exp(-eta())) ) / sin( 2. * atan(exp(-eta())) ) ); }
  double mass() { return m_pair->InvariantMass(); }

  int getRoiNum() { return m_roi; }

  const DiMuonTool* tool() const { return m_pair; }

 private:

  const Rec::TrackParticle *m_match;
  DiMuonTool *m_pair;
  int m_roi;

};



inline std::ostream& operator<<( std::ostream& s, const JpsiTrack& t) {
  s << *((Track*)(&t)) << "m_pair address: " << t.tool() << "  " << std::endl;
  return s << "\t" << *t.tool();
}

}

#endif
