/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local includes
#include "G4AtlasStackingAction.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  G4AtlasStackingAction::G4AtlasStackingAction()
  {
  }

  //---------------------------------------------------------------------------
  // Classify a new track
  //---------------------------------------------------------------------------
  G4ClassificationOfNewTrack
  G4AtlasStackingAction::ClassifyNewTrack(const G4Track* track)
  {
    // Loop over my actions and apply each one in turn
    // TODO: decide how to properly handle multiple stacking action results.
    // Maybe we just need to forbid it. See ATLASSIM-2421.
    G4ClassificationOfNewTrack classification = fUrgent;
    for(auto action : m_actions){
      classification = action->ClassifyNewTrack(track);
      if(classification == fKill) return fKill;
    }
    return classification;
  }

  //---------------------------------------------------------------------------
  // Start a new stack stage
  //---------------------------------------------------------------------------
  void G4AtlasStackingAction::NewStage()
  {
    // Loop over my actions and apply each one in turn
    for(auto action : m_actions){
      action->NewStage();
    }
  }

  //---------------------------------------------------------------------------
  // Start a new event
  //---------------------------------------------------------------------------
  void G4AtlasStackingAction::PrepareNewEvent()
  {
    // Loop over my actions and apply each one in turn
    for(auto action : m_actions){
      action->PrepareNewEvent();
    }
  }

  //---------------------------------------------------------------------------
  // Add one action to the action list
  //---------------------------------------------------------------------------
  void G4AtlasStackingAction::addAction(G4UserStackingAction* action)
  {
    m_actions.push_back(action);
  }

} // namespace G4UA
