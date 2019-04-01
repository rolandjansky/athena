/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_G4UA__PHYSICSVALIATIONUSERACTIONTOOL_H
#define ISF_GEANT4TOOLS_G4UA__PHYSICSVALIATIONUSERACTIONTOOL_H

#include "G4AtlasTools/UserActionToolBase.h"
#include "PhysicsValidationUserAction.h"

namespace G4UA
{

  namespace iGeant4
  {

    /// @class PhysicsValidationUserActionTool
    /// @brief Place holder tool
    ///
    /// @author John Chapman
    ///
    class PhysicsValidationUserActionTool :
      public UserActionToolBase<PhysicsValidationUserAction>
    {

    public:
      /// Standard constructor
      PhysicsValidationUserActionTool(const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent);
    protected:
      /// Create an action for this thread
      virtual std::unique_ptr<PhysicsValidationUserAction>
      makeAndFillAction(G4AtlasUserActions&) override final;
    private:
      /// Configuration parameters
      PhysicsValidationUserAction::Config m_config;
    }; // class PhysicsValidationUserActionTool

  }// iGeant4

} // namespace G4UA

#endif //ISF_GEANT4TOOLS_G4UA__PHYSICSVALIATIONUSERACTIONTOOL_H
