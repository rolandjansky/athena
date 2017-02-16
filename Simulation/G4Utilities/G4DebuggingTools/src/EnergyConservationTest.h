/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_EnergyConservationTest_H
#define G4DEBUGGINGTOOLS_EnergyConservationTest_H


#include <string>

#include "G4AtlasInterfaces/IPreTrackingAction.h"
#include "G4AtlasInterfaces/IPostTrackingAction.h"
#include "G4AtlasInterfaces/IEndEventAction.h"
#include "G4AtlasInterfaces/ISteppingAction.h"
#include "AthenaBaseComps/AthMessaging.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"

namespace G4UA{ 
  class EnergyConservationTest: 
  public AthMessaging, public IPreTrackingAction,  public IPostTrackingAction,  public IEndEventAction,  public ISteppingAction
  {
  public:
    EnergyConservationTest();
    virtual void preTracking(const G4Track*) override;
    virtual void postTracking(const G4Track*) override;
    virtual void endOfEvent(const G4Event*) override;
    virtual void processStep(const G4Step*) override;

  private:
    typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
    /// Pointer to StoreGate (event store by default)
    mutable StoreGateSvc_t m_evtStore;
    /// Pointer to StoreGate (detector store by default)
    mutable StoreGateSvc_t m_detStore;
    double e_in , e_out , e_dep;

  }; // class EnergyConservationTest
} // namespace G4UA 



#endif // G4DEBUGGINGTOOLS_EnergyConservationTest_H
