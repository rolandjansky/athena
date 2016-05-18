/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES__G4UA_IENDEVENTACTIONTOOL_H
#define G4ATLASINTERFACES__G4UA_IENDEVENTACTIONTOOL_H

// Framework includes
#include "GaudiKernel/IAlgTool.h"

// Local includes
#include "G4AtlasInterfaces/IEndEventAction.h"

namespace G4UA
{

  /// @class IEndEventActionTool
  /// @brief Abstract interface for tools that manage ATLAS end-of-event
  /// custom actions.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class IEndEventActionTool : virtual public IAlgTool
  {

    public:

      /// Return the action for current thread.
      virtual IEndEventAction* getEndEventAction() = 0;

      /// Interface declaration
      static const InterfaceID& interfaceID() {
        static const InterfaceID iid_IEndEventTool("G4UA::IEndEventActionTool", 1, 0);
        return iid_IEndEventTool;
      }

  }; // class IEndEventActionTool

} // namespace G4UA

#endif
