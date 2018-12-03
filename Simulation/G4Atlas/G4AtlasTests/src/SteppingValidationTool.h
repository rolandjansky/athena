/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTESTS_G4UA__STEPPINGVALIDATIONTOOL_H 
#define G4ATLASTESTS_G4UA__STEPPINGVALIDATIONTOOL_H 
#include "G4AtlasInterfaces/IG4RunActionTool.h"
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "SteppingValidation.h"
namespace G4UA{ 

  /// @class SteppingValidationTool
  /// @brief a tool to manage the SteppingValidation action in hive
  ///
  /// creates one instance of the action per thread
  ///
  /// @author Andrea Di Simone  

  class SteppingValidationTool: 
  public ActionToolBase<SteppingValidation>,
    public IG4RunActionTool, public IG4EventActionTool, public IG4SteppingActionTool
  {
    
  public:
    /// standard tool ctor
    SteppingValidationTool(const std::string& type, const std::string& name,const IInterface* parent);
    /// gets the run action
    virtual G4UserRunAction* getRunAction() override final
    { return static_cast<G4UserRunAction*>( getAction() ); }
    /// gets the event action
    virtual G4UserEventAction* getEventAction() override final
    { return static_cast<G4UserEventAction*>( getAction() ); }
    /// gets the stepping action
    virtual G4UserSteppingAction* getSteppingAction() override final
    { return static_cast<G4UserSteppingAction*>( getAction() ); }
    /// Gaudi interface handling
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
  protected:
    /// creates the instance of the action for this thread
    virtual std::unique_ptr<SteppingValidation> makeAction() override final;
  private:
  }; // class SteppingValidationTool
  

} // namespace G4UA 
#endif
