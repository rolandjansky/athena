/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKG4USERACTIONS_G4UA__MATERIALSTEPRECORDERTOOL_H 
#define TRKG4USERACTIONS_G4UA__MATERIALSTEPRECORDERTOOL_H 

#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4RunActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "TrkG4UserActions/MaterialStepRecorder.h"

namespace G4UA{ 
  
  /// @class MaterialStepRecorderTool
  /// @brief Tool which manages the MaterialStepRecorder action
  ///
  /// @author Andrea Di Simone
  ///

  class MaterialStepRecorderTool: 
  public ActionToolBase<MaterialStepRecorder>,
    public IG4EventActionTool, public IG4RunActionTool,  public IG4SteppingActionTool
  {
    
  public:
    /// Standard constructor
    MaterialStepRecorderTool(const std::string& type, const std::string& name,const IInterface* parent);
    /// Retrieve the event action
    virtual G4UserEventAction* getEventAction() override final 
    { return static_cast<G4UserEventAction*>( getAction() ); }
    /// Retrieve the run action
    virtual G4UserRunAction* getRunAction() override final 
    { return static_cast<G4UserRunAction*>( getAction() ); }
    /// Retrieve the stepping action
    virtual G4UserSteppingAction* getSteppingAction() override final 
    { return static_cast<G4UserSteppingAction*>( getAction() ); }
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
  protected:
    /// Create an action for this thread
    virtual std::unique_ptr<MaterialStepRecorder> makeAction() override final;
  private:
  }; // class MaterialStepRecorderTool
  
  
} // namespace G4UA 
#endif
