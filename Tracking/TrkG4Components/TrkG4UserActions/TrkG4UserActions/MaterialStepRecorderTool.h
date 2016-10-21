/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKG4USERACTIONS_G4UA__MATERIALSTEPRECORDERTOOL_H 
#define TRKG4USERACTIONS_G4UA__MATERIALSTEPRECORDERTOOL_H 

#include "G4AtlasInterfaces/IBeginEventActionTool.h"
#include "G4AtlasInterfaces/IEndEventActionTool.h"
#include "G4AtlasInterfaces/IBeginRunActionTool.h"
#include "G4AtlasInterfaces/ISteppingActionTool.h"
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
    public IBeginEventActionTool,  public IEndEventActionTool,  public IBeginRunActionTool,  public ISteppingActionTool
  {
    
  public:
    /// Standard constructor
    MaterialStepRecorderTool(const std::string& type, const std::string& name,const IInterface* parent);
    /// Retrieve the BoE action
    virtual IBeginEventAction* getBeginEventAction() override final 
    { return static_cast<IBeginEventAction*>( getAction() ); }
    /// Retrieve the EoE action
    virtual IEndEventAction* getEndEventAction() override final 
    { return static_cast<IEndEventAction*>( getAction() ); }
    /// Retrieve the BoR action
    virtual IBeginRunAction* getBeginRunAction() override final 
    { return static_cast<IBeginRunAction*>( getAction() ); }
    /// Retrieve the stepping action
    virtual ISteppingAction* getSteppingAction() override final 
    { return static_cast<ISteppingAction*>( getAction() ); }
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
  protected:
    /// Create an action for this thread
    virtual std::unique_ptr<MaterialStepRecorder> makeAction() override final;
  private:
  }; // class MaterialStepRecorderTool
  
  
} // namespace G4UA 
#endif
