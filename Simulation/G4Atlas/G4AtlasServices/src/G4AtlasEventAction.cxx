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
    for(IBeginEventAction* action : m_beginEventActions){
      action->beginOfEvent(event);
    }
  }

  //---------------------------------------------------------------------------
  // End-event action
  //---------------------------------------------------------------------------
  void G4AtlasEventAction::EndOfEventAction(const G4Event* event)
  {
    // Loop over my post-actions and apply each one in turn
    for(IEndEventAction* action : m_endEventActions){
      action->endOfEvent(event);
    }
  }

  //---------------------------------------------------------------------------
  // Add one action to the list
  //---------------------------------------------------------------------------
  void G4AtlasEventAction::addBeginEventAction(IBeginEventAction* action)
  {
    m_beginEventActions.push_back(action);
  }

  //---------------------------------------------------------------------------
  // Add one action to the list
  //---------------------------------------------------------------------------
  void G4AtlasEventAction::addEndEventAction(IEndEventAction* action)
  {
    m_endEventActions.push_back(action);
  }

} // namespace G4UA
