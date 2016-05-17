/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA__ENERGYCONSERVATIONTESTTOOL_H 
#define G4DEBUGGINGTOOLS_G4UA__ENERGYCONSERVATIONTESTTOOL_H 

#include "G4AtlasInterfaces/IPreTrackingActionTool.h"
#include "G4AtlasInterfaces/IPostTrackingActionTool.h"
#include "G4AtlasInterfaces/IEndEventActionTool.h"
#include "G4AtlasInterfaces/ISteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "EnergyConservationTest.h"

namespace G4UA{ 
  
  class EnergyConservationTestTool: 
  public ActionToolBase<EnergyConservationTest>,
    public IPreTrackingActionTool,  public IPostTrackingActionTool,  public IEndEventActionTool,  public ISteppingActionTool
  {

  public:
    EnergyConservationTestTool(const std::string& type, const std::string& name,const IInterface* parent);
    virtual IPreTrackingAction* getPreTrackingAction() override final 
    { return static_cast<IPreTrackingAction*>( getAction() ); }
    virtual IPostTrackingAction* getPostTrackingAction() override final 
    { return static_cast<IPostTrackingAction*>( getAction() ); }
    virtual IEndEventAction* getEndEventAction() override final 
    { return static_cast<IEndEventAction*>( getAction() ); }
    virtual ISteppingAction* getSteppingAction() override final 
    { return static_cast<ISteppingAction*>( getAction() ); }
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
  protected:
    virtual std::unique_ptr<EnergyConservationTest> makeAction() override final;
  private:
  }; // class EnergyConservationTestTool
} // namespace G4UA 
#endif
