/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES__G4UA_IBEGINEVENTACTIONTOOL_H
#define G4ATLASINTERFACES__G4UA_IBEGINEVENTACTIONTOOL_H

// Framework includes
#include "GaudiKernel/IAlgTool.h"

// Local includes
#include "G4AtlasInterfaces/IBeginEventAction.h"

namespace G4UA
{

  /// @class IBeginEventActionTool
  /// @brief Abstract interface for tools that manage ATLAS begin-of-event
  /// custom actions.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class IBeginEventActionTool : virtual public IAlgTool
  {

    public:

      /// Return the action for current thread.
      virtual IBeginEventAction* getBeginEventAction() = 0;

      /// Interface declaration
      static const InterfaceID& interfaceID() {
        static const InterfaceID iid_IBeginEventTool("G4UA::IBeginEventActionTool", 1, 0);
        return iid_IBeginEventTool;
      }

  }; // class IBeginEventActionTool

} // namespace G4UA

#endif
