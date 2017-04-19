/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_HyperspaceCatcher_H
#define G4DEBUGGINGTOOLS_HyperspaceCatcher_H


#include <string>

class G4VSolid;
class G4Track;


#include "G4AtlasInterfaces/IBeginRunAction.h"
#include "G4AtlasInterfaces/ISteppingAction.h"
#include "AthenaBaseComps/AthMessaging.h"

namespace G4UA{ 
  
  class HyperspaceCatcher: 
  public AthMessaging, public IBeginRunAction,  public ISteppingAction
  {
  public:
    
    struct Config
    {
      int treatmentLevel=0;
      int killAfter=0;
    };
    
    HyperspaceCatcher(const Config& config);
    virtual void beginOfRun(const G4Run*) override;
    virtual void processStep(const G4Step*) override;

  private:
    Config m_config;
    G4VSolid * m_world;
    int m_killCount;
  }; // class HyperspaceCatcher
} // namespace G4UA 

#endif //G4DEBUGGINGTOOLS_HyperspaceCatcher_H
