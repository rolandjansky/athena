/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUIRKS_G4UA__DEBUGSTEPPINGACTIONTOOL_H
#define QUIRKS_G4UA__DEBUGSTEPPINGACTIONTOOL_H

#include "G4AtlasTools/UserActionToolBase.h"
#include "DebugSteppingAction.h"

namespace G4UA
{

  /// @class DebugSteppingActionTool
  /// @brief a class to manage the DebugSteppingAction action
  /// @author Andrea Di Simone
  ///
  class DebugSteppingActionTool :  public UserActionToolBase<DebugSteppingAction>
  {

    public:

      /// Standard constructor
      DebugSteppingActionTool(const std::string& type,
                              const std::string& name,
                              const IInterface* parent);

    protected:

      /// creates one action per thread
      virtual std::unique_ptr<DebugSteppingAction>
      makeAndFillAction(G4AtlasUserActions&) override final;

    private:

      /// the config for the action
      DebugSteppingAction::Config m_config;

  }; // class DebugSteppingActionTool

} // namespace G4UA

#endif
