/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SingleLVCopyBuilder.h"
#include "Geo2G4/LogicalVolume.h"
#include "SingleLogicalVolumeFactory.h"
#include "SimHelpers/ServiceAccessor.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "G4PVPlacement.hh"
#include "G4ReflectionFactory.hh"
#include "globals.hh"
#include <iostream>

LogicalVolume* SingleLVCopyBuilder::Build(const PVConstLink theGeoPhysVolume, OpticalVolumesMap* /*optical_volumes*/) const
{
  static SingleLogicalVolumeFactory LVFactory;

  const GeoLogVol * geoLog = theGeoPhysVolume->getLogVol();
  ATH_MSG_DEBUG ( "Start converting volume "<<geoLog->getName() );

  G4LogicalVolume * theG4LogVolume = LVFactory.Build(geoLog);

  if (theGeoPhysVolume->getNChildVols()==0) return theG4LogVolume;
  if (theG4LogVolume->GetNoDaughters() ) return theG4LogVolume;
  //
  // Loop over the children of the GeoVolume and place them
  //
  for(size_t ii = 0; ii<theGeoPhysVolume->getNChildVols(); ii++)  {
    std::string nameChild = theGeoPhysVolume->getNameOfChildVol(ii);
    //
    // Get the id from GeoModel
    Query<int> Qint = theGeoPhysVolume->getIdOfChildVol(ii);
    int id = 90999;
    if(Qint.isValid() ) id = Qint;
    //
    // Get the child Phys volume ii
    //
    PVConstLink theGeoPhysChild = theGeoPhysVolume->getChildVol(ii);
    //
    // Build the child
    //
    G4LogicalVolume* theG4LogChild = Build(theGeoPhysChild);

    // Get its transform
    const G4Transform3D theG4Position(theGeoPhysVolume->getXToChildVol(ii));

    if (nameChild == "ANON") nameChild=theG4LogChild->GetName();
    //     log<<MSG::VERBOSE<<"\t Positioning "<<theG4LogChild->GetName()<<
    //       " into "<<theG4LogVolume->GetName()<< " with name "<<nameChild
    //        << "and Id:" << id <<endreq;
    G4ReflectionFactory::Instance()->Place(theG4Position,
                                           nameChild,
                                           theG4LogChild,
                                           theG4LogVolume,
                                           false,
                                           id);

  }

  return theG4LogVolume;
}
