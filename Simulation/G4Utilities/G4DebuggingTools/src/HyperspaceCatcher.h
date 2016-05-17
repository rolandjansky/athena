/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_HyperspaceCatcher_H
#define G4DEBUGGINGTOOLS_HyperspaceCatcher_H

#include "G4AtlasTools/UserActionBase.h"

#include <string>

class G4VSolid;
class G4Track;

class HyperspaceCatcher final:  public UserActionBase {

  public:
  HyperspaceCatcher(const std::string& type, const std::string& name, const IInterface* parent);

   virtual void Step(const G4Step*) override;
   virtual void BeginOfRun(const G4Run*) override;
   virtual StatusCode queryInterface(const InterfaceID&, void**) override;

   // This method is passed a const G4Track, so it can't really kill things
   // void PreUserTrackingAction(const G4Track* aTrack);

  private:
   //   void initAndSetup();
   int m_killCount;
   G4VSolid * m_world;
   int m_treatmentLevel;
   int m_killAfter;
};

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
