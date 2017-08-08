/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS__G4UA_G4TRACKCOUNTERTOOL_H
#define G4USERACTIONS__G4UA_G4TRACKCOUNTERTOOL_H

// STL includes
#include <string>

// Infrastructure includes
#include "G4AtlasInterfaces/IBeginEventActionTool.h"
#include "G4AtlasInterfaces/IPreTrackingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"

// Local includes
#include "G4TrackCounter.h"

namespace G4UA
{

  /// @class G4TrackCounterTool
  /// @brief Tool which manages the G4TrackCounter action.
  /// This tool creates the G4TrackCounter for each thread and merges the track
  /// counts from all threads in finalization.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class G4TrackCounterTool : public ActionToolBaseReport<G4TrackCounter>,
                             public IBeginEventActionTool,
                             public IPreTrackingActionTool
  {

    public:

      /// Standard constructor
      G4TrackCounterTool(const std::string& type, const std::string& name,
                         const IInterface* parent);

      /// Initialize tool - temporarily just for debugging
      virtual StatusCode initialize() override;

      /// Finalize and merge results from all threads
      virtual StatusCode finalize() override;

      /// Retrieve the begin-event action interface
      virtual IBeginEventAction* getBeginEventAction() override final
      { return static_cast<IBeginEventAction*>( getAction() ); }

      /// Retrieve the pre-tracking action interface
      virtual IPreTrackingAction* getPreTrackingAction() override final
      { return static_cast<IPreTrackingAction*>( getAction() ); }

    protected:

      /// Create action for this thread
      virtual std::unique_ptr<G4TrackCounter> makeAction() override final;

  }; // class G4TrackCounterTool

} // namespace G4UA

#endif
