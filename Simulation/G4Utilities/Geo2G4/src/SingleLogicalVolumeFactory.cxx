/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SingleLogicalVolumeFactory.h"
#include "Geo2G4SolidFactory.h"
#include "GeoMaterial2G4/Geo2G4MaterialFactory.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "SimHelpers/ServiceAccessor.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"

SingleLogicalVolumeFactory::SingleLogicalVolumeFactory(): m_msg("SingleLogicalVolumeFactory")
{
}

typedef std::map<std::string, G4LogicalVolume*, std::less<std::string> > vMap;

G4LogicalVolume *SingleLogicalVolumeFactory::Build(const GeoLogVol* theLog) const
{
  static Geo2G4SolidFactory theSolidFactory;
  static Geo2G4MaterialFactory theMaterialFactory;
  static vMap volumeList;
  //
  // Get Material from GeoModel
  //
  const std::string& n= theLog->getName();
  if (volumeList.find(n) == volumeList.end()) {
    G4LogicalVolume *theG4Log=0;
    G4Material* theG4Mat=theMaterialFactory.Build(theLog->getMaterial());

    G4VSolid * theG4Solid = theSolidFactory.Build(theLog->getShape());

    ATH_MSG_DEBUG("Building logical volume (single) "<<theLog->getName()<< "   " << theG4Mat);
    theG4Log = new G4LogicalVolume(theG4Solid,
                                   theG4Mat,
                                   theLog->getName(),
                                   0,0,0);
    volumeList[n] = theG4Log;
    return theG4Log;
  }
  return volumeList[n];
}
