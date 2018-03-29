/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CheckActivation.h"

#include "G4TransportationManager.hh"
#include "G4Navigator.hh"

namespace G4UA{ 


CheckActivation::CheckActivation(){;
}

void CheckActivation::BeginOfEventAction(const G4Event*){

  G4TransportationManager *tm = G4TransportationManager::GetTransportationManager();
  tm->GetNavigatorForTracking()->CheckMode(true);
}

} // namespace G4UA 
