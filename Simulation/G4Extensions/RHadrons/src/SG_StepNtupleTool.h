/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RHADRONS_G4UA__SG_STEPNTUPLETOOL_H 
#define RHADRONS_G4UA__SG_STEPNTUPLETOOL_H 

#include "G4AtlasInterfaces/IG4RunActionTool.h"
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "SG_StepNtuple.h"

namespace G4UA{ 

  /// @class SG_StepNtupleTool
  /// @brief Tool which manages the SG_StepNtuple  user action.
  ///  creates one SG_StepNtuple instance per thread
  /// @author Andrea Di Simone

  class SG_StepNtupleTool: 
  public ActionToolBase<SG_StepNtuple>,
    public IG4RunActionTool, public IG4EventActionTool, public IG4SteppingActionTool
  {
    
  public:
    /// standard tool ctor
    SG_StepNtupleTool(const std::string& type, const std::string& name,const IInterface* parent);
    /// retrieves run action
    virtual G4UserRunAction* getRunAction() override final 
    { return static_cast<G4UserRunAction*>( getAction() ); }
    /// retrieves event action
    virtual G4UserEventAction* getEventAction() override final 
    { return static_cast<G4UserEventAction*>( getAction() ); }
    /// retrieves stepping action
    virtual G4UserSteppingAction* getSteppingAction() override final 
    { return static_cast<G4UserSteppingAction*>( getAction() ); }
    /// gaudi interface manipulation
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
  protected:
    /// creates the action instances
    virtual std::unique_ptr<SG_StepNtuple> makeAction() override final;
  private:
  }; // class SG_StepNtupleTool
  
  
} // namespace G4UA 
#endif
