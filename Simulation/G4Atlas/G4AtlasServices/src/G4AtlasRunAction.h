/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS__G4UA_G4ATLASRUNACTION_H
#define G4ATLASTOOLS__G4UA_G4ATLASRUNACTION_H

// STL includes
#include <vector>

// Geant4 includes
#include "G4UserRunAction.hh"


namespace G4UA
{

  /// @class G4AtlasRunAction
  /// @brief ATLAS subclass of the G4 run action.
  ///
  /// Maintains a list of custom actions for the beginning and end of an run
  /// and when invoked by Geant4 will forward the call to each of them in turn.
  ///
  /// @todo TODO lifetime management of wrapper actions.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class G4AtlasRunAction : public G4UserRunAction
  {

    public:

      /// Constructor
      G4AtlasRunAction();

      /// @brief Geant4 method for pre-run action.
      /// This method forwards the G4 call onto each of its
      /// pre-run ATLAS actions.
      void BeginOfRunAction(const G4Run* run) override final;

      /// @brief Geant4 method for post-run action.
      /// This method forwards the G4 call onto each of its
      /// post-run ATLAS actions.
      void EndOfRunAction(const G4Run* run) override final;

      /// Add one action to the list
      void addRunAction(G4UserRunAction* action);

    private:

      /// List of ATLAS run actions
      std::vector<G4UserRunAction*> m_runActions;

  }; // class G4AtlasRunAction

}

#endif
