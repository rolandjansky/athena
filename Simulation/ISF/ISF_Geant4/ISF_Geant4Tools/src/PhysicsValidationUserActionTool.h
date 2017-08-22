/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_G4UA__PHYSICSVALIATIONUSERACTIONTOOL_H
#define ISF_GEANT4TOOLS_G4UA__PHYSICSVALIATIONUSERACTIONTOOL_H
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4RunActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasInterfaces/IG4TrackingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "PhysicsValidationUserAction.h"

namespace G4UA{
  
  namespace iGeant4{
    
    
    /// @class PhysicsValidationUserActionTool
    /// @brief Place holder tool
    ///
    /// @author John Chapman
    ///
    
    class PhysicsValidationUserActionTool:
    public ActionToolBase<PhysicsValidationUserAction>,
      public IG4EventActionTool, public IG4RunActionTool, public IG4SteppingActionTool, public IG4TrackingActionTool
    {
      
    public:
      /// Standard constructor
      PhysicsValidationUserActionTool(const std::string& type, const std::string& name,const IInterface* parent);
      /// Retrieve the event action
      virtual G4UserEventAction* getEventAction() override final
      { return static_cast<G4UserEventAction*>( getAction() ); }
      /// Retrieve the run action
      virtual G4UserRunAction* getRunAction() override final
      { return static_cast<G4UserRunAction*>( getAction() ); }
      /// Retrieve the stepping action
      virtual G4UserSteppingAction* getSteppingAction() override final
      { return static_cast<G4UserSteppingAction*>( getAction() ); }
      /// Retrieve the tracking action
      virtual G4UserTrackingAction* getTrackingAction() override final
      { return static_cast<G4UserTrackingAction*>( getAction() ); }
      
      /// Query interface for gaudi
      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
    protected:
      /// Create an action for this thread
      virtual std::unique_ptr<PhysicsValidationUserAction> makeAction() override final;
    private:
      /// Configuration parameters
      PhysicsValidationUserAction::Config m_config;
    }; // class PhysicsValidationUserActionTool
    
  }// iGeant4
} // namespace G4UA
#endif //ISF_GEANT4TOOLS_G4UA__PHYSICSVALIATIONUSERACTIONTOOL_H
