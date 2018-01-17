/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PVNotifier.h"

#include "G4PhysicalVolumeStore.hh"
#include "G4VPhysicalVolume.hh"

#include "G4GeometryNotifierSvc.h"

PVNotifier::PVNotifier(G4GeometryNotifierSvc* gs)
  : m_notifierSvc(gs)
{
  G4PhysicalVolumeStore* store = G4PhysicalVolumeStore::GetInstance();
  store->SetNotifier(this);
}

void PVNotifier::NotifyRegistration()
{
  G4PhysicalVolumeStore* store = G4PhysicalVolumeStore::GetInstance();
  unsigned int current = store->size();
  G4VPhysicalVolume* lV = (*store)[current-1];
  std::string temp1 = m_notifierSvc->GetCurrentDetectorName() + "::";
  std::string temp2 = lV->GetName().substr(0, temp1.size());
  if (temp1 != temp2)
    lV->SetName(temp1 + lV->GetName());
}

void PVNotifier::NotifyDeRegistration()
{
}
