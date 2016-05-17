/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS__G4UA_G4ATLASEVENTACTION_H
#define G4ATLASTOOLS__G4UA_G4ATLASEVENTACTION_H

// STL includes
#include <vector>

// Geant4 includes
#include "G4UserEventAction.hh"

// Local includes
#include "G4AtlasInterfaces/IBeginEventAction.h"
#include "G4AtlasInterfaces/IEndEventAction.h"


namespace G4UA
{

  /// @class G4AtlasEventAction
  /// @brief ATLAS subclass of the G4 event action.
  ///
  /// Maintains a list of custom actions for the beginning and end of an event
  /// and when invoked by Geant4 will forward the call to each of them in turn.
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
      void addBeginEventAction(IBeginEventAction* action);

      /// Add one action to the list
      void addEndEventAction(IEndEventAction* action);

    private:

      /// List of ATLAS begin-event actions
      std::vector<IBeginEventAction*> m_beginEventActions;

      /// List of ATLAS end-event actions
      std::vector<IEndEventAction*> m_endEventActions;

  }; // class G4AtlasEventAction

}

#endif
