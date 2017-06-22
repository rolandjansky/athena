/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RHADRONS_G4UA__SG_STEPNTUPLETOOL_H 
#define RHADRONS_G4UA__SG_STEPNTUPLETOOL_H 

#include "G4AtlasInterfaces/IBeginRunActionTool.h"
#include "G4AtlasInterfaces/IBeginEventActionTool.h"
#include "G4AtlasInterfaces/IEndEventActionTool.h"
#include "G4AtlasInterfaces/ISteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "SG_StepNtuple.h"

namespace G4UA{ 

  /// @class SG_StepNtupleTool
  /// @brief Tool which manages the SG_StepNtuple  user action.
  ///  creates one SG_StepNtuple instance per thread
  /// @author Andrea Di Simone

  class SG_StepNtupleTool: 
  public ActionToolBase<SG_StepNtuple>,
    public IBeginRunActionTool,  public IBeginEventActionTool,  public IEndEventActionTool,  public ISteppingActionTool
  {
    
  public:
    /// standard tool ctor
    SG_StepNtupleTool(const std::string& type, const std::string& name,const IInterface* parent);
    /// retrieves BoR action
    virtual IBeginRunAction* getBeginRunAction() override final 
    { return static_cast<IBeginRunAction*>( getAction() ); }
    /// retrieves BoE action
    virtual IBeginEventAction* getBeginEventAction() override final 
    { return static_cast<IBeginEventAction*>( getAction() ); }
    /// retrieves EoE action
    virtual IEndEventAction* getEndEventAction() override final 
    { return static_cast<IEndEventAction*>( getAction() ); }
    /// retrieves stepping action
    virtual ISteppingAction* getSteppingAction() override final 
    { return static_cast<ISteppingAction*>( getAction() ); }
    /// gaudi interface manipulation
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
  protected:
    /// creates the action instances
    virtual std::unique_ptr<SG_StepNtuple> makeAction() override final;
  private:
  }; // class SG_StepNtupleTool
  
  
} // namespace G4UA 
#endif
