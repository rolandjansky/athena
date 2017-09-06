/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA_COSMICPERIGEEACTIONTOOL_H
#define G4USERACTIONS_G4UA_COSMICPERIGEEACTIONTOOL_H

// System includes
#include <string>

// Infrastructure includes
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4TrackingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"

// Local includes
#include "G4UserActions/CosmicPerigeeAction.h"

namespace G4UA
{

  /// @class CosmicPerigeeActionTool
  /// @brief Tool which manages the CosmicPerigeeAction action.
  ///
  /// Create the CosmicPerigeeAction for each worker thread
  ///
  /// @author Andrea Di Simone
  ///
  class CosmicPerigeeActionTool : public ActionToolBase<CosmicPerigeeAction>,
                                  public IG4SteppingActionTool,
                                  public IG4EventActionTool,
                                  public IG4TrackingActionTool
  {

    public:

      /// Standard constructor
      CosmicPerigeeActionTool(const std::string& type, const std::string& name,
		              const IInterface* parent);


      /// Retrieve the stepping action interface
      virtual G4UserSteppingAction* getSteppingAction() override final
      { return static_cast<G4UserSteppingAction*>( getAction() ); }

      /// Retrieve the event action interface
      virtual G4UserEventAction* getEventAction() override final
      { return static_cast<G4UserEventAction*>( getAction() ); }

      /// Retrieve the preTracking action interface
      virtual G4UserTrackingAction* getTrackingAction() override final
      { return static_cast<G4UserTrackingAction*>( getAction() ); }

    protected:

      /// Create action for this thread
      virtual std::unique_ptr<CosmicPerigeeAction> makeAction() override final;

    private:

      /// Configuration parameters
      CosmicPerigeeAction::Config m_config;

  }; // class CosmicPerigeeActionTool

} // namespace G4UA

#endif
