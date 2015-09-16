/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LVNotifier.h"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"

#include "DetectorGeometrySvc.h"

LVNotifier::LVNotifier(DetectorGeometrySvc* gs):m_detGeoSvc(gs)
{
  G4LogicalVolumeStore *store=G4LogicalVolumeStore::GetInstance();
  store->SetNotifier(this);
}
void LVNotifier::NotifyRegistration()
{
  G4LogicalVolumeStore *store=G4LogicalVolumeStore::GetInstance();
  G4LogicalVolume *lV=store->back();
  lV->SetName(m_detGeoSvc->GetCurrentDetectorName()+"::"+lV->GetName());
}

void LVNotifier::NotifyDeRegistration()
{
}
