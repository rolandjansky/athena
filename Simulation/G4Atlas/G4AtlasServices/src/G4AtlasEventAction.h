/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS__G4UA_G4ATLASEVENTACTION_H
#define G4ATLASTOOLS__G4UA_G4ATLASEVENTACTION_H

// STL includes
#include <vector>

// Geant4 includes
#include "G4UserEventAction.hh"


namespace G4UA
{

  /// @class G4AtlasEventAction
  /// @brief ATLAS subclass of the G4 event action.
  ///
  /// Maintains a list of custom actions for the beginning and end of an event
  /// and when invoked by Geant4 will forward the call to each of them in turn.
  ///
  /// @todo TODO lifetime management of wrapper actions.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class G4AtlasEventAction : public G4UserEventAction
  {

    public:

      /// Constructor
      G4AtlasEventAction();

      /// @brief Geant4 method for pre-event action.
      /// This method forwards the G4 call onto each of its
      /// pre-event ATLAS actions.
      void BeginOfEventAction(const G4Event* event) override final;

      /// @brief Geant4 method for post-event action.
      /// This method forwards the G4 call onto each of its
      /// post-event ATLAS actions.
      void EndOfEventAction(const G4Event* event) override final;

      /// Add one action to the list
      void addEventAction(G4UserEventAction* action);

    private:

      /// List of ATLAS event actions
      std::vector<G4UserEventAction*> m_eventActions;

  }; // class G4AtlasEventAction

}

#endif
