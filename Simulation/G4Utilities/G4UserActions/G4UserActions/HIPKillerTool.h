/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__HIPKILLERTOOL_H
#define G4USERACTIONS_G4UA__HIPKILLERTOOL_H
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/UserActionToolBase.h"
#include "G4UserActions/HIPKiller.h"

namespace G4UA
{

  /// @brief Tool which manages the HIPKiller user action.
  ///
  class HIPKillerTool : public UserActionToolBase<HIPKiller>
  {

    public:

      /// Standard constructor
      HIPKillerTool(const std::string& type, const std::string& name,
                    const IInterface* parent);

    protected:

      /// Create the action for the current thread
      virtual std::unique_ptr<HIPKiller>
      makeAndFillAction(G4AtlasUserActions&) override final;

  }; // class HIPKillerTool

} // namespace G4UA

#endif
