/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_FixG4CreatorProcess_H
#define G4UserActions_FixG4CreatorProcess_H

// Geant4 includes
#include "G4UserSteppingAction.hh"

namespace G4UA
{

  class FixG4CreatorProcess : public G4UserSteppingAction
  {
    public:
      FixG4CreatorProcess();
      virtual void UserSteppingAction(const G4Step*) override final;
  }; // class FixG4CreatorProcess

} // namespace G4UA

#endif
