/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__HIPKILLERTOOL_H
#define G4USERACTIONS_G4UA__HIPKILLERTOOL_H
#include "G4AtlasInterfaces/ISteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "G4UserActions/HIPKiller.h"

namespace G4UA
{

  /// @brief Tool which manages the HIPKiller user action.
  ///
  class HIPKillerTool: public ActionToolBase<HIPKiller>, public ISteppingActionTool
  {

    public:

      /// Standard constructor
      HIPKillerTool(const std::string& type, const std::string& name,
                    const IInterface* parent);

      virtual ISteppingAction* getSteppingAction() override final
      { return static_cast<ISteppingAction*>( getAction() ); }

    protected:

      virtual std::unique_ptr<HIPKiller> makeAction() override final;

  }; // class HIPKillerTool

} // namespace G4UA

#endif
