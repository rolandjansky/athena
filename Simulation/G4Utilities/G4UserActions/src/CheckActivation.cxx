/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/CheckActivation.h"
#include "G4TransportationManager.hh"
#include "G4Navigator.hh"

static CheckActivation ts1("CheckActivation");

void CheckActivation::BeginOfEventAction(const G4Event* /*anEvent*/)
{
  G4TransportationManager *tm = G4TransportationManager::GetTransportationManager();
  tm->GetNavigatorForTracking()->CheckMode(true);
}
