/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AtlasAlg_AthenaTrackingActionTool_H
#define G4AtlasAlg_AthenaTrackingActionTool_H

// Infrastructure includes
#include "G4AtlasInterfaces/IG4TrackingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"

// Local includes
#include "AthenaTrackingAction.h"

namespace G4UA
{

  /// @class AthenaTrackingActionTool
  /// @brief Tool which manages the AthenaTrackingAction
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class AthenaTrackingActionTool : public ActionToolBase<AthenaTrackingAction>,
                                   public IG4TrackingActionTool
  {

    public:

      /// Standard constructor
      AthenaTrackingActionTool(const std::string& type, const std::string& name,
                               const IInterface* parent);

      /// Initialize the tool (just for debugging printout)
      virtual StatusCode initialize() override;

      /// Retrieve the tracking action
      virtual G4UserTrackingAction* getTrackingAction() override final
      { return static_cast<G4UserTrackingAction*>( getAction() ); }

    protected:

      /// Create an action for this thread
      virtual std::unique_ptr<AthenaTrackingAction> makeAction() override final;

    private:

      /// The saving level for secondaries.
      int m_secondarySavingLevel;
      /// The level in the G4 volume hierarchy at which can we find the sub-detector name
      int m_subDetVolLevel;

  }; // class AthenaTrackingActionTool

} // namespace G4UA

#endif
