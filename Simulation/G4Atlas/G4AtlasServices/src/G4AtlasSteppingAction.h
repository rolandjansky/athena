/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS__G4UA_G4ATLASSTEPPINGACTION_H
#define G4ATLASTOOLS__G4UA_G4ATLASSTEPPINGACTION_H

// System includes
#include <vector>

// Geant4 includes
#include "G4UserSteppingAction.hh"


namespace G4UA
{

  /// @class G4AtlasSteppingAction
  /// @brief Atlas subclass of the G4 stepping action.
  ///
  /// This object maintains a list of custom actions and when invoked by
  /// Geant4 will forward the call to each of them in turn.
  ///
  /// @todo TODO lifetime management of wrapper actions.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class G4AtlasSteppingAction : public G4UserSteppingAction
  {

    public:

      /// Constructor
      G4AtlasSteppingAction();

      /// @brief Geant4 method for processing one step.
      /// This method will forward the call onto each of the actions
      /// in its private list.
      void UserSteppingAction(const G4Step* step) override final;

      /// @brief Add one action to the list
      void addAction(G4UserSteppingAction* action);

    private:

      /// List of ATLAS stepping actions
      std::vector<G4UserSteppingAction*> m_actions;

  }; // class G4AtlasSteppingAction

} // namespace G4UA

#endif
