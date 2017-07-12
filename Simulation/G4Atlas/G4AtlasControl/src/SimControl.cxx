/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasControl/SimControl.h"

// Geant4 includes
#include "G4RunManager.hh"

// STL includes
#include <stdexcept>

SimControl::SimControl()
{
}

SimControl::~SimControl()
{
}

void SimControl::initializeG4(bool isMT) const
{
  auto rm = G4RunManager::GetRunManager();
  if (rm) {
    rm->Initialize();
    // Initialization differs slightly in multi-threading.
    // TODO: add more details about why this is here.
    if(!isMT && rm->ConfirmBeamOnCondition()) {
      rm->RunInitialization();
    }
  }
  else throw std::runtime_error("Run manager retrieval has failed");
}
