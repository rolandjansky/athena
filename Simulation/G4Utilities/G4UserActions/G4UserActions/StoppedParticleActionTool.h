/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__STOPPEDPARTICLEACTIONTOOL_H
#define G4USERACTIONS_G4UA__STOPPEDPARTICLEACTIONTOOL_H

#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "G4UserActions/StoppedParticleAction.h"

namespace G4UA
{

  /// @brief Tool which manages the StoppedParticleAction
  ///
  class StoppedParticleActionTool: public ActionToolBase<StoppedParticleAction>,
                                   public IG4SteppingActionTool
  {

    public:

      /// Standard constructor
      StoppedParticleActionTool(const std::string& type, const std::string& name,
                                const IInterface* parent);

      virtual G4UserSteppingAction* getSteppingAction() override final
      { return static_cast<G4UserSteppingAction*>( getAction() ); }

    protected:

      virtual std::unique_ptr<StoppedParticleAction> makeAction() override final;

  }; // class StoppedParticleActionTool

} // namespace G4UA

#endif
