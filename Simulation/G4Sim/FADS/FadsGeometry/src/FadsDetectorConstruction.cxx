/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsGeometry/FadsDetectorConstruction.h"

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PhysicalVolumeStore.hh"

#include <iostream>

namespace FADS {

  FadsDetectorConstruction::FadsDetectorConstruction(): topEnvelope(0),topVolume(0)
  {
    geometryCheckLevel=0;
    isGeometry=false;
  }
  FadsDetectorConstruction::~FadsDetectorConstruction()
  {
  }

  G4VPhysicalVolume* FadsDetectorConstruction::Construct()
  {
    // std::cout<<"FadsDetectorConstruction::Construct running here"<<std::endl;
    if (!topEnvelope)
      {
        std::cout <<"FadsDetectorConstruction: the top envelope is not set!!!"<<std::endl;
        std::cout <<"                          program can't continue, aborting..."<<std::endl;
        abort();
      }
    if (!topVolume)
      topVolume=new G4PVPlacement(0,G4ThreeVector(),topEnvelope,
                                  topEnvelope->GetName(),0,false,0);
    isGeometry=true;
    if (geometryCheckLevel) CheckGeometry();
    return topVolume;
  }
  void FadsDetectorConstruction::SetEnvelope(G4LogicalVolume *lV)
  {
    topEnvelope=lV;
  }
  void FadsDetectorConstruction::SetPhysicalVolume(G4VPhysicalVolume *pV)
  {
    topVolume=pV;
  }
  G4LogicalVolume *FadsDetectorConstruction::GetEnvelope()
  {
    return topEnvelope;
  }

  void FadsDetectorConstruction::SetGeometryCheckLevel(int i)
  {
    geometryCheckLevel=i;
  }

  void FadsDetectorConstruction::CheckGeometry()
  {
    if (!isGeometry)
      {
        std::cout<<"FadsDetectorConstruction: not ready yet for CheckGeometry()"<<std::endl;
        return;
      }
    G4PhysicalVolumeStore *store=G4PhysicalVolumeStore::GetInstance();
    unsigned int current=store->size();
    std::cout<<std::endl<<" FadsDetectorConstruction::CheckGeometry will be scanning "<<
      current<<" volumes for detecting overlaps: stay tuned..."<<std::endl<<std::endl;
    std::cout<<"\t currently using "<<geometryCheckLevel<<" points per volume"<<std::endl;
    int last=0;
    for (unsigned int i=0;i<current;i++)
      {
        int done = int(100*(double(i)/current));
        if (!(done%5))
          {
            if (done!=last)
              {
                std::cout <<std::endl<<"\t\t"<<done<<"% done"<<std::endl;
                last=done;
              }
            else
              if (!(i%10)) std::cout <<"."<<std::flush;
          }
        bool overlap=false;
        G4VPhysicalVolume *vol=(*store)[i];
        G4PVPlacement* pvol=dynamic_cast<G4PVPlacement*>(vol);
        if (pvol) overlap=pvol->CheckOverlaps(geometryCheckLevel);
        if (overlap)
          {
            std::cout<<" PhysicalVolumeCheck\t ERROR ";
            std::cout<<" Physical Volume name "<<pvol->GetName();
            std::cout<<" tagged for overlaps!!! ";
            std::cout<<" ERROR !!!"<<std::endl;
          }
      }
    std::cout <<std::endl<<" CheckGeometry done"<<std::endl;
    return;

  }

}
