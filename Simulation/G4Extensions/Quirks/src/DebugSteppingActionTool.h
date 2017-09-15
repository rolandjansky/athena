/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUIRKS_G4UA__DEBUGSTEPPINGACTIONTOOL_H 
#define QUIRKS_G4UA__DEBUGSTEPPINGACTIONTOOL_H 
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "DebugSteppingAction.h"
namespace G4UA{ 
  /// @class DebugSteppingActionTool
  /// @brief a class to manage the DebugSteppingAction action
  /// @author Andrea Di Simone
  
  class DebugSteppingActionTool: 
  public ActionToolBase<DebugSteppingAction>,
    public IG4SteppingActionTool
    {
      
    public:
      DebugSteppingActionTool(const std::string& type, const std::string& name,const IInterface* parent);
      /// retrieves the stepping action
      virtual G4UserSteppingAction* getSteppingAction() override final 
      { return static_cast<G4UserSteppingAction*>( getAction() ); }
      /// gaudi interface handling
      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
    protected:
      /// creates one action per thread
      virtual std::unique_ptr<DebugSteppingAction> makeAction() override final;
    private:
      /// the config for the action
      DebugSteppingAction::Config m_config;
    }; // class DebugSteppingActionTool
  
  
} // namespace G4UA 
#endif
