/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA__STEPNTUPLETOOL_H 
#define G4DEBUGGINGTOOLS_G4UA__STEPNTUPLETOOL_H 
#include "G4AtlasInterfaces/IBeginEventActionTool.h"
#include "G4AtlasInterfaces/IEndEventActionTool.h"
#include "G4AtlasInterfaces/ISteppingActionTool.h"
#include "G4AtlasInterfaces/IBeginRunActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "StepNtuple.h"

namespace G4UA{ 
  
  
  /// @class StepNtupleTool
  /// @brief Tool which manages the StepNtuple action.
  ///
  /// Create the StepNtuple for each worker thread
  ///
  /// @author Andrea Di Simone
  ///
  
  class StepNtupleTool: 
  public ActionToolBase<StepNtuple>,
    public IBeginEventActionTool,  public IEndEventActionTool,  public ISteppingActionTool,  public IBeginRunActionTool
  {
    
  public:
    /// standard tool ctor
    StepNtupleTool(const std::string& type, const std::string& name,const IInterface* parent);
    /// return the BoE action
    virtual IBeginEventAction* getBeginEventAction() override final 
    { return static_cast<IBeginEventAction*>( getAction() ); }
    /// return the EoE action
    virtual IEndEventAction* getEndEventAction() override final 
    { return static_cast<IEndEventAction*>( getAction() ); }
    /// return the stepping action
    virtual ISteppingAction* getSteppingAction() override final 
    { return static_cast<ISteppingAction*>( getAction() ); }
    /// return the BoR action
    virtual IBeginRunAction* getBeginRunAction() override final 
    { return static_cast<IBeginRunAction*>( getAction() ); }
    /// gaudi's interface handling
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
  protected:
    /// Create action for this thread
    virtual std::unique_ptr<StepNtuple> makeAction() override final;
    
  }; // class StepNtupleTool
  
  
} // namespace G4UA 
#endif
