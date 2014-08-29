/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef StoppedParticleAction_h
#define StoppedParticleAction_h

#include "FadsActions/UserAction.h"
#include <string>
class TrackFastSimSD;

class StoppedParticleAction: public FADS::UserAction
{
 public:

  StoppedParticleAction(std::string name) : UserAction(name) , m_fsSD(0) , m_init(false) {}
  
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);
  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);
  void SteppingAction(const G4Step*);

 protected: 

  bool isSUSYParticle(const int) const;
  TrackFastSimSD * m_fsSD;  
  bool m_init;
};

#endif

