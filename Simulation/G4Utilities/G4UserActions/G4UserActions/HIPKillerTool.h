/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__HIPKILLERTOOL_H
#define G4USERACTIONS_G4UA__HIPKILLERTOOL_H
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "G4UserActions/HIPKiller.h"

namespace G4UA
{

  /// @brief Tool which manages the HIPKiller user action.
  ///
  class HIPKillerTool: public ActionToolBase<HIPKiller>, public IG4SteppingActionTool
  {

    public:

      /// Standard constructor
      HIPKillerTool(const std::string& type, const std::string& name,
                    const IInterface* parent);

      virtual G4UserSteppingAction* getSteppingAction() override final
      { return static_cast<G4UserSteppingAction*>( getAction() ); }

    protected:

      virtual std::unique_ptr<HIPKiller> makeAction() override final;

  }; // class HIPKillerTool

} // namespace G4UA

#endif
