/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_G4UA__PHYSICSVALIATIONUSERACTIONTOOL_H
#define ISF_GEANT4TOOLS_G4UA__PHYSICSVALIATIONUSERACTIONTOOL_H
#include "G4AtlasInterfaces/IBeginEventActionTool.h"
#include "G4AtlasInterfaces/IEndEventActionTool.h"
#include "G4AtlasInterfaces/IBeginRunActionTool.h"
#include "G4AtlasInterfaces/ISteppingActionTool.h"
#include "G4AtlasInterfaces/IPreTrackingActionTool.h"
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
      public IBeginEventActionTool,  public IEndEventActionTool,  public IBeginRunActionTool,  public ISteppingActionTool,  public IPreTrackingActionTool
    {

    public:
      /// Standard constructor
      PhysicsValidationUserActionTool(const std::string& type, const std::string& name,const IInterface* parent);
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
      /// Retrieve the preTracking action
      virtual IPreTrackingAction* getPreTrackingAction() override final
      { return static_cast<IPreTrackingAction*>( getAction() ); }

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
