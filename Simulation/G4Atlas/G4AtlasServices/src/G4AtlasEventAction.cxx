/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local includes
#include "G4AtlasEventAction.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  G4AtlasEventAction::G4AtlasEventAction()
  {
  }

  //---------------------------------------------------------------------------
  // Begin-event action
  //---------------------------------------------------------------------------
  void G4AtlasEventAction::BeginOfEventAction(const G4Event* event)
  {
    // Loop over my pre-actions and apply each one in turn
    for(auto action : m_eventActions){
      action->BeginOfEventAction(event);
    }
  }

  //---------------------------------------------------------------------------
  // End-event action
  //---------------------------------------------------------------------------
  void G4AtlasEventAction::EndOfEventAction(const G4Event* event)
  {
    // Loop over my post-actions and apply each one in turn
    for(auto action : m_eventActions){
      action->EndOfEventAction(event);
    }
  }

  //---------------------------------------------------------------------------
  // Add one action to the list
  //---------------------------------------------------------------------------
  void G4AtlasEventAction::addEventAction(G4UserEventAction* action)
  {
    m_eventActions.push_back(action);
  }

} // namespace G4UA
