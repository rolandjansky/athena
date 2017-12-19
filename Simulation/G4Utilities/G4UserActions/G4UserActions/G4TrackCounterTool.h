/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS__G4UA_G4TRACKCOUNTERTOOL_H
#define G4USERACTIONS__G4UA_G4TRACKCOUNTERTOOL_H

// Infrastructure includes
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4TrackingActionTool.h"
#include "G4AtlasTools/UserActionToolBase.h"

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
  class G4TrackCounterTool : public UserActionToolBase<G4TrackCounter>
  {

    public:

      /// Standard constructor
      G4TrackCounterTool(const std::string& type, const std::string& name,
                         const IInterface* parent);

      /// Initialize tool
      virtual StatusCode initialize() override final;

      /// Finalize and merge results from all threads
      virtual StatusCode finalize() override final;

    protected:

      /// Create action for this thread
      virtual std::unique_ptr<G4TrackCounter>
      makeAndFillAction(G4AtlasUserActions&) override final;

  }; // class G4TrackCounterTool

} // namespace G4UA

#endif
