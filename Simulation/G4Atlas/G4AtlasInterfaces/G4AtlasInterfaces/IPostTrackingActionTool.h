/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES__G4UA_IPOSTTRACKINGACTIONTOOL_H
#define G4ATLASINTERFACES__G4UA_IPOSTTRACKINGACTIONTOOL_H

// Framework includes
#include "GaudiKernel/IAlgTool.h"

// Local includes
#include "G4AtlasInterfaces/IPostTrackingAction.h"

namespace G4UA
{

  /// @class IPostTrackingActionTool
  /// @brief Abstract interface for tools that manage ATLAS post-tracking
  /// actions.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class IPostTrackingActionTool : virtual public IAlgTool
  {

    public:

      /// Return the action for current thread.
      virtual IPostTrackingAction* getPostTrackingAction() = 0;

      /// Interface declaration
      static const InterfaceID& interfaceID() {
        static const InterfaceID iid_IPostTrkTool("G4UA::IPostTrackingActionTool", 1, 0);
        return iid_IPostTrkTool;
      }

  }; // class IPostTrackingActionTool

} // namespace G4UA

#endif
