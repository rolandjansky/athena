/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA__STEPNTUPLETOOL_H 
#define G4DEBUGGINGTOOLS_G4UA__STEPNTUPLETOOL_H 
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasInterfaces/IG4RunActionTool.h"
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
  
  class StepNtupleTool : public ActionToolBase<StepNtuple>,
                         public IG4EventActionTool,
                         public IG4SteppingActionTool,
                         public IG4RunActionTool
  {
    
  public:
    /// standard tool ctor
    StepNtupleTool(const std::string& type, const std::string& name,const IInterface* parent);
    /// return the event action
    virtual G4UserEventAction* getEventAction() override final
    { return static_cast<G4UserEventAction*>( getAction() ); }
    /// return the stepping action
    virtual G4UserSteppingAction* getSteppingAction() override final
    { return static_cast<G4UserSteppingAction*>( getAction() ); }
    /// return the run action
    virtual G4UserRunAction* getRunAction() override final
    { return static_cast<G4UserRunAction*>( getAction() ); }
    /// gaudi's interface handling
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
  protected:
    /// Create action for this thread
    virtual std::unique_ptr<StepNtuple> makeAction() override final;
    
  }; // class StepNtupleTool
  
  
} // namespace G4UA 
#endif
