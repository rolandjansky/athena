/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES__G4UA_IG4EVENTACTIONTOOL_H
#define G4ATLASINTERFACES__G4UA_IG4EVENTACTIONTOOL_H

// Framework includes
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class G4UserEventAction;

namespace G4UA
{

  /// @class IG4EventActionTool
  /// @brief Abstract interface for tools that construct ATLAS G4 event actions.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class IG4EventActionTool : virtual public IAlgTool
  {

    public:

      /// Return the action for current thread.
      virtual G4UserEventAction* getEventAction() = 0;

      /// Creates the InterfaceID and interfaceID() method
      DeclareInterfaceID(G4UA::IG4EventActionTool, 1, 0);

  }; // class IG4EventActionTool

} // namespace G4UA

#endif
