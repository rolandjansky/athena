/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_IUSERACTIONTOOL_H
#define G4ATLASINTERFACES_IUSERACTIONTOOL_H

// System includes
#include <vector>

// Framework includes
#include "GaudiKernel/IAlgTool.h"

// Geant4 forward declarations
class G4UserRunAction;
class G4UserEventAction;
class G4UserTrackingAction;
class G4UserSteppingAction;
class G4UserStackingAction;

namespace G4UA
{

  /// @class G4AtlasUserActions
  /// @brief Struct for passing around user actions.
  ///
  /// Tools push their actions onto the appropriate lists.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  struct G4AtlasUserActions
  {
    std::vector<G4UserRunAction*> runActions;
    std::vector<G4UserEventAction*> eventActions;
    std::vector<G4UserTrackingAction*> trackingActions;
    std::vector<G4UserSteppingAction*> steppingActions;
    std::vector<G4UserStackingAction*> stackingActions;
  };

  /// @class IUserActionTool
  /// @brief Abstract interface for tools that construct G4 user actions.
  ///
  /// This interface declares a single method for concrete tools to implement
  /// in order to plug user actions into Geant4 and the UserActionSvc.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class IUserActionTool : virtual public IAlgTool
  {

    public:

      /// Constructs a user action and populates the user action lists with it.
      virtual StatusCode fillUserAction(G4AtlasUserActions& userActions) = 0;

      /// Declare this interface to the framework
      DeclareInterfaceID(IUserActionTool, 1, 0);

  }; // class IUserActionTool

} // namespace G4UA

#endif // G4ATLASINTERFACES_IUSERACTIONTOOL_H
