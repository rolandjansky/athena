/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FadsParallelWorld_H
#define FadsParallelWorld_H

#include "globals.hh"
#include "G4VUserParallelWorld.hh"

class G4LogicalVolume;

namespace FADS {

  class FadsParallelWorld : public G4VUserParallelWorld
  {
  public:
    FadsParallelWorld(G4String worldName);
    virtual ~FadsParallelWorld();
    void SetTreeTop(G4LogicalVolume*);
    virtual void Construct();

  private:
    G4LogicalVolume* treeTop;
    std::string parallelName;
  };

}


#endif
