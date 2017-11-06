/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA__ENERGYCONSERVATIONTESTTOOL_H 
#define G4DEBUGGINGTOOLS_G4UA__ENERGYCONSERVATIONTESTTOOL_H 

#include "G4AtlasInterfaces/IG4TrackingActionTool.h"
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "EnergyConservationTest.h"

namespace G4UA{ 
  
  class EnergyConservationTestTool: 
  public ActionToolBase<EnergyConservationTest>,
    public IG4TrackingActionTool,  public IG4EventActionTool,  public IG4SteppingActionTool
  {

  public:
    EnergyConservationTestTool(const std::string& type, const std::string& name,const IInterface* parent);
    virtual G4UserTrackingAction* getTrackingAction() override final
    { return static_cast<G4UserTrackingAction*>( getAction() ); }
    virtual G4UserEventAction* getEventAction() override final
    { return static_cast<G4UserEventAction*>( getAction() ); }
    virtual G4UserSteppingAction* getSteppingAction() override final
    { return static_cast<G4UserSteppingAction*>( getAction() ); }
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
  protected:
    virtual std::unique_ptr<EnergyConservationTest> makeAction() override final;
  private:
  }; // class EnergyConservationTestTool
} // namespace G4UA 
#endif
