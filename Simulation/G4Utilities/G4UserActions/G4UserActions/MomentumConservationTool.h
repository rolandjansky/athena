/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__MOMENTUMCONSERVATIONTOOL_H
#define G4USERACTIONS_G4UA__MOMENTUMCONSERVATIONTOOL_H

#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "G4UserActions/MomentumConservation.h"

namespace G4UA
{

  /// @brief Tool which manages the MomentumConservation user action.
  ///
  class MomentumConservationTool : public ActionToolBase<MomentumConservation>,
                                   public IG4EventActionTool,
                                   public IG4SteppingActionTool
  {

    public:

      /// Standard constructor
      MomentumConservationTool(const std::string& type, const std::string& name,
                               const IInterface* parent);

      virtual G4UserEventAction* getEventAction() override final
      { return static_cast<G4UserEventAction*>( getAction() ); }

      virtual G4UserSteppingAction* getSteppingAction() override final
      { return static_cast<G4UserSteppingAction*>( getAction() ); }

    protected:

      virtual std::unique_ptr<MomentumConservation> makeAction() override final;

  }; // class MomentumConservationTool

} // namespace G4UA

#endif
