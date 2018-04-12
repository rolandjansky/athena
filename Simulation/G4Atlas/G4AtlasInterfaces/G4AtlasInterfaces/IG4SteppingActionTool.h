/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES__G4UA_IG4STEPPINGACTIONTOOL_H
#define G4ATLASINTERFACES__G4UA_IG4STEPPINGACTIONTOOL_H

// Framework includes
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class G4UserSteppingAction;

namespace G4UA
{

  /// @class IG4SteppingActionTool
  /// @brief Abstract interface for tools that construct G4 stepping actions.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class IG4SteppingActionTool : virtual public IAlgTool
  {
    
    public:

      /// @brief Return the action for current thread.
      virtual G4UserSteppingAction* getSteppingAction() = 0;

      /// Creates the InterfaceID and interfaceID() method
      DeclareInterfaceID(G4UA::IG4SteppingActionTool, 1, 0);

  }; // class IG4SteppingActionTool

} // namespace G4UA

#endif
