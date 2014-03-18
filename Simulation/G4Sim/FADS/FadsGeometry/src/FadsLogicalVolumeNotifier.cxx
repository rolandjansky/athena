/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsGeometry/FadsLogicalVolumeNotifier.h"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"

#include "FadsGeometry/GeometryManager.h"

#include <iostream>

namespace FADS {

  FadsLogicalVolumeNotifier::FadsLogicalVolumeNotifier()
  {
    std::cout<<"\t  LogicalVolumeNotifier being built!!!!"<<std::endl;
    G4LogicalVolumeStore *store=G4LogicalVolumeStore::GetInstance();
    store->SetNotifier(this);
  }

  void FadsLogicalVolumeNotifier::NotifyRegistration()
  {
    static GeometryManager* gManager=FADS::GeometryManager::GetGeometryManager();
    G4LogicalVolumeStore *store=G4LogicalVolumeStore::GetInstance();
    G4LogicalVolume *lV=store->back();
    std::string vName=gManager->GetCurrentDetectorName()+"::"+lV->GetName();
    lV->SetName(vName);
    gManager->RegisterVolume(lV);
    G4Material *mat __attribute__((unused)) =lV->GetMaterial();
  }

  void FadsLogicalVolumeNotifier::NotifyDeRegistration()
  {
    // std::cout<<"This is LogicalVolumeNotifier::NotifyDeregistration()"<<std::endl;
  }

}
