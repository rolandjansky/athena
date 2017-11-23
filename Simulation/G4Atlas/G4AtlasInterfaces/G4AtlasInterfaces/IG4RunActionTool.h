/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES__G4UA_IG4RUNACTIONTOOL_H
#define G4ATLASINTERFACES__G4UA_IG4RUNACTIONTOOL_H

// Framework includes
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class G4UserRunAction;

namespace G4UA
{

  /// @class IG4RunActionTool
  /// @brief Abstract interface for tools that construct G4 run actions.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class IG4RunActionTool : virtual public IAlgTool
  {

    public:

      /// Return the action for current thread.
      virtual G4UserRunAction* getRunAction() = 0;

      /// Creates the InterfaceID and interfaceID() method
      DeclareInterfaceID(G4UA::IG4RunActionTool, 1, 0);

  }; // class IG4RunActionTool

} // namespace G4UA

#endif
