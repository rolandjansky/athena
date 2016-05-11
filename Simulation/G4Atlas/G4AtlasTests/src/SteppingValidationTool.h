/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTESTS_G4UA__STEPPINGVALIDATIONTOOL_H 
#define G4ATLASTESTS_G4UA__STEPPINGVALIDATIONTOOL_H 
#include "G4AtlasInterfaces/IBeginRunActionTool.h"
#include "G4AtlasInterfaces/IEndEventActionTool.h"
#include "G4AtlasInterfaces/IBeginEventActionTool.h"
#include "G4AtlasInterfaces/ISteppingActionTool.h"
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
    public IBeginRunActionTool,  public IEndEventActionTool,  public IBeginEventActionTool,  public ISteppingActionTool
  {
    
  public:
    /// standard tool ctor
    SteppingValidationTool(const std::string& type, const std::string& name,const IInterface* parent);
    /// gets the BoR action
    virtual IBeginRunAction* getBeginRunAction() override final
    { return static_cast<IBeginRunAction*>( getAction() ); }
    /// gets the EoE action
    virtual IEndEventAction* getEndEventAction() override final 
    { return static_cast<IEndEventAction*>( getAction() ); }
    /// gets the BoE action
    virtual IBeginEventAction* getBeginEventAction() override final 
    { return static_cast<IBeginEventAction*>( getAction() ); }
    /// gets the stepping action
    virtual ISteppingAction* getSteppingAction() override final 
    { return static_cast<ISteppingAction*>( getAction() ); }
    /// Gaudi interface handling
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
  protected:
    /// creates the instance of the action for this thread
    virtual std::unique_ptr<SteppingValidation> makeAction() override final;
  private:
  }; // class SteppingValidationTool
  

} // namespace G4UA 
#endif
