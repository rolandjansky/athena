/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local includes
#include "G4AtlasTrackingAction.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  G4AtlasTrackingAction::G4AtlasTrackingAction()
  {
  }

  //---------------------------------------------------------------------------
  // Pre-tracking action
  //---------------------------------------------------------------------------
  void G4AtlasTrackingAction::PreUserTrackingAction(const G4Track* trk)
  {
    // Loop over my pre-actions and apply each one in turn
    for(auto action : m_trackActions){
      action->PreUserTrackingAction(trk);
    }
  }

  //---------------------------------------------------------------------------
  // Post-tracking action
  //---------------------------------------------------------------------------
  void G4AtlasTrackingAction::PostUserTrackingAction(const G4Track* trk)
  {
    // Loop over my post-actions and apply each one in turn
    for(auto action : m_trackActions){
      action->PostUserTrackingAction(trk);
    }
  }

  //---------------------------------------------------------------------------
  // Add one action to the list
  //---------------------------------------------------------------------------
  void G4AtlasTrackingAction::addTrackAction(G4UserTrackingAction* action)
  {
    m_trackActions.push_back(action);
  }

} // namespace G4UA
