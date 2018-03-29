/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_HyperspaceCatcher_H
#define G4DEBUGGINGTOOLS_HyperspaceCatcher_H


#include <string>

class G4VSolid;
class G4Track;


#include "G4UserRunAction.hh"
#include "G4UserSteppingAction.hh"
#include "AthenaBaseComps/AthMessaging.h"

namespace G4UA{ 
  
  class HyperspaceCatcher: 
  public AthMessaging, public G4UserRunAction, public G4UserSteppingAction
  {
  public:
    
    struct Config
    {
      int treatmentLevel=0;
      int killAfter=0;
    };
    
    HyperspaceCatcher(const Config& config);
    virtual void BeginOfRunAction(const G4Run*) override;
    virtual void UserSteppingAction(const G4Step*) override;

  private:
    Config m_config;
    G4VSolid * m_world;
    int m_killCount;
  }; // class HyperspaceCatcher
} // namespace G4UA 

#endif //G4DEBUGGINGTOOLS_HyperspaceCatcher_H
