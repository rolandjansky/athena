/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__MOMENTUMCONSERVATIONTOOL_H
#define G4USERACTIONS_G4UA__MOMENTUMCONSERVATIONTOOL_H

// Infrastructure includes
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "MomentumConservation.h"

namespace G4UA
{

  /// @brief Tool which manages the MomentumConservation user action.
  ///
  class MomentumConservationTool : public UserActionToolBase<MomentumConservation>
  {

    public:

      /// Standard constructor
      MomentumConservationTool(const std::string& type, const std::string& name,
                               const IInterface* parent);

    protected:

      /// Create the action for the current thread
      virtual std::unique_ptr<MomentumConservation>
      makeAndFillAction(G4AtlasUserActions&) override final;

  }; // class MomentumConservationTool

} // namespace G4UA

#endif
