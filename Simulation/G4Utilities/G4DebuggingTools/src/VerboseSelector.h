/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_VerboseSelector_H
#define G4DEBUGGINGTOOLS_VerboseSelector_H


#include <string>


#include "G4UserSteppingAction.hh"
#include "G4UserTrackingAction.hh"
#include "G4UserEventAction.hh"
#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

namespace G4UA{ 

class VerboseSelector: 
  public AthMessaging, public G4UserEventAction, public G4UserSteppingAction, public G4UserTrackingAction
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
    virtual void UserSteppingAction(const G4Step*) override;
    virtual void PreUserTrackingAction(const G4Track*) override;
    virtual void PostUserTrackingAction(const G4Track*) override;
    virtual void BeginOfEventAction(const G4Event*) override;

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
