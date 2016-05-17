/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local includes
#include "G4AtlasRunAction.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  G4AtlasRunAction::G4AtlasRunAction()
  {
  }

  //---------------------------------------------------------------------------
  // Begin-run action
  //---------------------------------------------------------------------------
  void G4AtlasRunAction::BeginOfRunAction(const G4Run* run)
  {
    // Loop over my pre-actions and apply each one in turn
    for(IBeginRunAction* action : m_beginRunActions){
      action->beginOfRun(run);
    }
  }

  //---------------------------------------------------------------------------
  // End-run action
  //---------------------------------------------------------------------------
  void G4AtlasRunAction::EndOfRunAction(const G4Run* run)
  {
    // Loop over my post-actions and apply each one in turn
    for(IEndRunAction* action : m_endRunActions){
      action->endOfRun(run);
    }
  }

  //---------------------------------------------------------------------------
  // Add one action to the list
  //---------------------------------------------------------------------------
  void G4AtlasRunAction::addBeginRunAction(IBeginRunAction* action)
  {
    m_beginRunActions.push_back(action);
  }

  //---------------------------------------------------------------------------
  // Add one action to the list
  //---------------------------------------------------------------------------
  void G4AtlasRunAction::addEndRunAction(IEndRunAction* action)
  {
    m_endRunActions.push_back(action);
  }

} // namespace G4UA
