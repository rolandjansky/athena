/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__STOPPEDPARTICLEACTIONTOOL_H
#define G4USERACTIONS_G4UA__STOPPEDPARTICLEACTIONTOOL_H

#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/UserActionToolBase.h"
#include "G4UserActions/StoppedParticleAction.h"

namespace G4UA
{

  /// @brief Tool which manages the StoppedParticleAction
  ///
  class StoppedParticleActionTool: public UserActionToolBase<StoppedParticleAction>
  {

    public:

      /// Standard constructor
      StoppedParticleActionTool(const std::string& type, const std::string& name,
                                const IInterface* parent);

    protected:

      /// Create the action for the current thread
      virtual std::unique_ptr<StoppedParticleAction>
      makeAndFillAction(G4AtlasUserActions&) override final;

  }; // class StoppedParticleActionTool

} // namespace G4UA

#endif
