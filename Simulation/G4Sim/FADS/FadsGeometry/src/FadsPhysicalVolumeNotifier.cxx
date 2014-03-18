/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsGeometry/FadsPhysicalVolumeNotifier.h"
#include "FadsGeometry/GeometryManager.h"

#include "G4PhysicalVolumeStore.hh"
#include "G4VPhysicalVolume.hh"

#include <iostream>

namespace FADS {

  FadsPhysicalVolumeNotifier::FadsPhysicalVolumeNotifier()
  {
    std::cout<<"\t PhysicalVolumeNotifier being built!!!"<<std::endl;
    G4PhysicalVolumeStore *store=G4PhysicalVolumeStore::GetInstance();
    store->SetNotifier(this);
  }

  void FadsPhysicalVolumeNotifier::NotifyRegistration()
  {
    static GeometryManager* gManager __attribute__((unused)) =FADS::GeometryManager::GetGeometryManager(); 
    static int nEntry=0;
    // std::cout<<"This is PhysicalVolumeNotifier::NotifyRegistration()"<<std::endl;
    nEntry++;
    if (nEntry>1)
      {
        G4PhysicalVolumeStore *store=G4PhysicalVolumeStore::GetInstance();
        unsigned int current=store->size();
        G4VPhysicalVolume *lV=(*store)[current-1];
        // std::cout<<" Physical Volume name "<<lV->GetName()<<std::endl;
        // std::cout<<" \t Checking for overlaps"<<std::endl;
        bool overlap=lV->CheckOverlaps(1000);
        if (overlap)
          {
            std::cout<<" PhysicalVolumeCheck\t ERROR ";
            std::cout<<" Physical Volume name "<<lV->GetName();
            std::cout<<" tagged for overlaps!!! ";
            std::cout<<" ERROR !!!"<<std::endl;
          }
      }
    // else
    //    std::cout<<" All OK"<<std::endl;
    // G4Material *mat=lV->GetMaterial();
    // std::cout<<" \t made of "<<mat->GetName()<<std::endl;
  }

  void FadsPhysicalVolumeNotifier::NotifyDeRegistration()
  {
    // std::cout<<"This is PhysicalVolumeNotifier::NotifyDeregistration()"<<std::endl;
  }

}
