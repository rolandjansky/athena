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
  Probe(const xAOD::IParticle& tagTrack, const xAOD::IParticle& probeTrack, int tagPassTrigger);
  
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
  // set probe closest match dr
  void dr_match(float in){ m_dr_match = in;}

  float sfweight(){ return m_sf_weight;}
  void sfweight(float w ){ m_sf_weight = w;}

  // trigger stuff
  void        SetCurrentTrigger(std::string TrigName) { m_CurrentTrigger = TrigName;}
  std::string GetCurrentTrigger()		      { return m_CurrentTrigger;}   

  bool HasSomeTrigger(void)    { return m_HasSomeTrigger; }
  void HasSomeTrigger(bool in) { m_HasSomeTrigger = in;   }

  bool HasSomeTrigger_HLT(void)    { return m_HasSomeTrigger_HLT; }
  void HasSomeTrigger_HLT(bool in) { m_HasSomeTrigger_HLT = in;   }

  int tagPassTrigger(){ return m_tagPassTrigger;}
  
  double dr_match() {return m_dr_match;}

  float dRL1;
  float dRHLT;

 private:

  bool m_isMatched;
  
  const xAOD::IParticle& m_tagTrack;
  const xAOD::IParticle& m_probeTrack;
  
  float m_sf_weight;     // for closure tests
  float m_dr_match;      // dR distance to closest match

  int m_tagPassTrigger;

  // trigger stuff
  std::string m_CurrentTrigger;
  bool        m_HasSomeTrigger;
  bool        m_HasSomeTrigger_HLT;
};

#endif
