/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_VerboseSelector_H
#define G4DEBUGGINGTOOLS_VerboseSelector_H

#include "G4AtlasTools/UserActionBase.h"

#include <string>

class VerboseSelector final: public UserActionBase {

  public:
  VerboseSelector(const std::string& type, const std::string& name, const IInterface* parent);

   virtual void EndOfEvent(const G4Event*) override;
   virtual void Step(const G4Step*) override;
   virtual void PreTracking(const G4Track* aTrack) override;
   virtual void PostTracking(const G4Track* aTrack) override;

   virtual StatusCode initialize() override;
   virtual StatusCode queryInterface(const InterfaceID&, void**) override;

  private:
   int m_targetEvent;
   int m_targetTrack;
   int m_targetBarcode;
   int m_verboseLevel;
   int m_evtCount;
   int m_verb;
   double m_Xmin, m_Xmax;
   double m_Ymin, m_Ymax;
   double m_Zmin, m_Zmax;
};


#include "G4AtlasInterfaces/ISteppingAction.h"
#include "G4AtlasInterfaces/IPreTrackingAction.h"
#include "G4AtlasInterfaces/IPostTrackingAction.h"
#include "G4AtlasInterfaces/IBeginEventAction.h"
#include "AthenaBaseComps/AthMessaging.h"

namespace G4UA{ 

class VerboseSelector: 
  public AthMessaging, public IBeginEventAction, public ISteppingAction,  public IPreTrackingAction,  public IPostTrackingAction
  {
    
  public:
    
    struct Config
    {
      
      int targetEvent=-1;
      int targetTrack=-1;
      int targetBarcode=-1;
      int verboseLevel=0;
      int verb=1;
      double Xmin=0, Xmax=0;
      double Ymin=0, Ymax=0;
      double Zmin=0, Zmax=0;
      
    };

    VerboseSelector(const Config& config);
    virtual void processStep(const G4Step*) override;
    virtual void preTracking(const G4Track*) override;
    virtual void postTracking(const G4Track*) override;
    virtual void beginOfEvent(const G4Event*) override;

  private:
    typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
    /// Pointer to StoreGate (event store by default)
    mutable StoreGateSvc_t m_evtStore;
    /// Pointer to StoreGate (detector store by default)
    mutable StoreGateSvc_t m_detStore;
    Config m_config;
    uint64_t m_evtCount;

  }; // class VerboseSelector
  
  
} // namespace G4UA 


#endif //G4DEBUGGINGTOOLS_VerboseSelector_H
