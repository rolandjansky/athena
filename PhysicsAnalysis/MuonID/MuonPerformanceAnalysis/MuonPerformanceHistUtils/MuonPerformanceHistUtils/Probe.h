/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Probe.h
#ifndef PROBE_H
#define PROBE_H

#include "xAODBase/IParticle.h"

class Probe {

 public:

  // constructor
  Probe(const xAOD::IParticle& tagTrack, const xAOD::IParticle& probeTrack);
  
  // destructor
  ~Probe();

  // access tag
  const xAOD::IParticle& tagTrack() {return m_tagTrack;}

  // access probe
  const xAOD::IParticle& probeTrack() {return m_probeTrack;}

  // probe pt
  double pt() {return m_probeTrack.pt();}

  // probe eta
  double eta() {return m_probeTrack.eta();}

  // probe phi
  double phi() {return m_probeTrack.phi();}

  // get probe matching status
  bool isMatched() {return m_isMatched;}

  // set probe matching status
  void isMatched(bool isM) {m_isMatched = isM;}

  float sfweight(){ return m_sf_weight;}
  void sfweight(float w ){ m_sf_weight = w;}

 private:

  bool m_isMatched;
  
  const xAOD::IParticle& m_tagTrack;
  const xAOD::IParticle& m_probeTrack;
  
  float m_sf_weight;     // for closure tests

};

#endif
