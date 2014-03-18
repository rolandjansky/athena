/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsGeometry/FadsParallelWorld.h"

#include "G4VUserParallelWorld.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"

#include <iostream>

namespace FADS {

  FadsParallelWorld::FadsParallelWorld(G4String worldName):G4VUserParallelWorld(worldName),treeTop(0),parallelName(worldName)
  {
    std::cout<<"new FadsParallelWorld "<<worldName<<" being created"<<std::endl;
  }
  FadsParallelWorld::~FadsParallelWorld()
  {
  }
  void FadsParallelWorld::SetTreeTop(G4LogicalVolume* lv)
  {
    treeTop=lv;
  }
  void FadsParallelWorld::Construct()
  {
    G4VPhysicalVolume* mainWorld = GetWorld();
    G4LogicalVolume* worldLogical = mainWorld->GetLogicalVolume();

    if (treeTop)
      new G4PVPlacement(0, G4ThreeVector(), treeTop,
                        parallelName+"Physical", worldLogical, 0, 0);
    else
      {
        std::cout<<"something wrong with "<<parallelName<<" parallel geometry!"<<std::endl;
        std::cout<<"\treeTop seems to be empty!!!"<<std::endl;
      }
  }

}
