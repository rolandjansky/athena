/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES__G4UA_IG4TRACKINGACTIONTOOL_H
#define G4ATLASINTERFACES__G4UA_IG4TRACKINGACTIONTOOL_H

// Framework includes
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class G4UserTrackingAction;

namespace G4UA
{

  /// @class IG4TrackingActionTool
  /// @brief Abstract interface for tools that construct G4 tracking actions.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class IG4TrackingActionTool : virtual public IAlgTool
  {

    public:

      /// Return the action for current thread.
      virtual G4UserTrackingAction* getTrackingAction() = 0;

      /// Interface declaration
      static const InterfaceID& interfaceID() {
        static const InterfaceID iid_IG4TrkTool("G4UA::IG4TrackingActionTool", 1, 0);
        return iid_IG4TrkTool;
      }

  }; // class IG4TrackingActionTool

} // namespace G4UA

#endif
