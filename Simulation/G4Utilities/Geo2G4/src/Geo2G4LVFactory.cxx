/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Geo2G4LVFactory.h"
#include "Geo2G4SolidFactory.h"
#include "GeoMaterial2G4/Geo2G4MaterialFactory.h"

#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"

#include "G4LogicalVolume.hh"
#include "G4Material.hh"

#include <iostream>
#include <map>

typedef std::map<const GeoLogVol*, G4LogicalVolume*, std::less<const GeoLogVol*> > leafVMap;
typedef std::map<const GeoVPhysVol*, G4LogicalVolume*, std::less<const GeoVPhysVol*> > branchVMap;

typedef std::map<const GeoFullPhysVol*, G4LogicalVolume*, std::less<const GeoFullPhysVol*> > fullPVMap;

Geo2G4LVFactory::Geo2G4LVFactory()
{
}

G4LogicalVolume* Geo2G4LVFactory::Build(const PVConstLink& thePhys,
                                        bool& descend) const
{
  static Geo2G4SolidFactory theSolidFactory;
  static Geo2G4MaterialFactory theMaterialFactory;

  static leafVMap sharedLeafLV;
  static branchVMap sharedBranchLV;
  static fullPVMap clonedLV;

  const GeoFullPhysVol* fullPV = dynamic_cast<const GeoFullPhysVol*>(&(*thePhys));
  const GeoFullPhysVol* clonePV=0;

  const GeoLogVol* theLog = thePhys->getLogVol();
  G4LogicalVolume *theG4Log=0;
  G4Material* theG4Mat=0;
  G4VSolid* theG4Solid=0;

  descend = true;
  bool putLeaf = false;
  bool putBranch = false;
  bool putFullPV = false;

  // Check if it is a leaf node of Geo tree
  if(thePhys->getNChildVols() == 0)
    {
      descend=false;

      if(sharedLeafLV.find(theLog) != sharedLeafLV.end())
        return sharedLeafLV[theLog];
      else // here supposed to be ---> else if(theLog->refCount() > 1)
        putLeaf = true;
    }
  // Work with the Full Physical Volumes
  else if(fullPV)
    {
      clonePV = fullPV->cloneOrigin();
      if (clonedLV.find(clonePV)==clonedLV.end())
        {
          if(clonePV) putFullPV = true;
        }
      else
        {
          descend = false;
          return clonedLV[clonePV];
        }
    }
  else
    {
      if(sharedBranchLV.find(&(*thePhys)) == sharedBranchLV.end())
        putBranch = true;
      else
        {
          descend = false;
          return sharedBranchLV[&(*thePhys)];
        }
    }
  // Actually build the G4Log

  theG4Mat=theMaterialFactory.Build(theLog->getMaterial());
  theG4Solid = theSolidFactory.Build(theLog->getShape(),theLog->getName());

  theG4Log = new G4LogicalVolume(theG4Solid,
                                 theG4Mat,
                                 theLog->getName(),
                                 0,0,0);

  if(putLeaf) sharedLeafLV[theLog] = theG4Log;
  if(putBranch) sharedBranchLV[&(*thePhys)] = theG4Log;
  if(putFullPV) clonedLV[clonePV] = theG4Log;

  return theG4Log;
}
