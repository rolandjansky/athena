/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_EnergyConservationTest_H
#define G4DEBUGGINGTOOLS_EnergyConservationTest_H

#include "G4AtlasTools/UserActionBase.h"

#include <string>

class EnergyConservationTest final: public UserActionBase{

  private:
   double e_in , e_out , e_dep;

  public:
   EnergyConservationTest(const std::string& type, const std::string& name, const IInterface* parent):
     UserActionBase(type,name,parent),e_in(0),e_out(0),e_dep(0) {};

   virtual void EndOfEvent(const G4Event*) override;
   virtual void Step(const G4Step*) override;
   virtual void PreTracking(const G4Track*) override;
   virtual void PostTracking(const G4Track*) override;

   virtual StatusCode queryInterface(const InterfaceID&, void**) override;


};


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
