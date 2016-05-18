/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES__G4UA_IPRETRACKINGACTIONTOOL_H
#define G4ATLASINTERFACES__G4UA_IPRETRACKINGACTIONTOOL_H

// Framework includes
#include "GaudiKernel/IAlgTool.h"

// Local includes
#include "G4AtlasInterfaces/IPreTrackingAction.h"

namespace G4UA
{

  /// @class IPreTrackingActionTool
  /// @brief Abstract interface for tools that manage ATLAS pre-tracking
  /// actions.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class IPreTrackingActionTool : virtual public IAlgTool
  {

    public:

      /// Return the action for current thread.
      virtual IPreTrackingAction* getPreTrackingAction() = 0;

      /// Interface declaration
      static const InterfaceID& interfaceID() {
        static const InterfaceID iid_IPreTrkTool("G4UA::IPreTrackingActionTool", 1, 0);
        return iid_IPreTrkTool;
      }

  }; // class IPreTrackingActionTool

} // namespace G4UA

#endif
