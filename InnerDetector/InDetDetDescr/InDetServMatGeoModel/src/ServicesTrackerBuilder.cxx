/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/ServicesTrackerBuilder.h"
#include "InDetServMatGeoModel/ServicesTracker.h"
#include "InDetServMatGeoModel/InDetServMatGeometryManager.h"

#include "AthenaKernel/MsgStreamMember.h"
#include <string>

#include <iostream>  // for DEBUG only
using namespace std;

ServicesTracker* ServicesTrackerBuilder::buildGeometry(const InDetServMatGeometryManager& geoMgr) const
{

  Athena::MsgStreamMember msg(Athena::Options::Eager,"ServiceTrackerBuilder");
  
  msg<< MSG::INFO << "Entering ServicesTrackerBuilder::buildGeometry" <<endmsg;

  ServicesTracker* tracker = new ServicesTracker(msg);
  msg << MSG::INFO << "Created new ServicesTracker()" << endmsg;

  std::string suffix;

  // Build pixel barrel layers
  for (int i = 0; i < geoMgr.pixelNumLayers(); i++) {
    int nChipsPerModule = geoMgr.pixelChipsPerModule( geoMgr.pixelBarrelModuleType(i));
    if (i == 0)      suffix = "L0";
    else if (i == 1) suffix = "L1";
    else             suffix = "Outer";

    int modulesPerStave = geoMgr.pixelModulesPerStave(i)/2; // we need only half-stave modules for services

    double layerRadius =  geoMgr.pixelLayerRadius(i);
    // Correction is 0 if no bent stave modules defined
    layerRadius -=  double(geoMgr.pixelBentStaveNModule(i))*geoMgr.pixelLadderModuleDeltaZ(i)*sin(geoMgr.pixelLadderBentStaveAngle(i) * M_PI / 180.0);

    tracker->constructBarrelLayer( layerRadius, 0.5*geoMgr.pixelLayerLength(i),
				   DetType::Pixel, i, geoMgr.pixelNumSectorsForLayer(i), suffix,
				   modulesPerStave, nChipsPerModule);
  }

  // Build endcap pixel layers

  for (int i = 0; i < geoMgr.pixelNumDisks(); i++) {
    int nModulesPerSector = geoMgr.pixelModulesPerEndcapSector(i);
    int nChipsPerModule = geoMgr.pixelChipsPerModuleForDisk(i);

    suffix = "Outer"; // endcap pixel disks use same services as outer barrel pixel
    tracker->constructEndcapLayer( geoMgr.pixelDiskZ(i), geoMgr.pixelDiskRMin(i), 
				   geoMgr.pixelDiskRMax(i), DetType::Pixel, i,
				   geoMgr.pixelEndcapNumSectorsForLayer(i), suffix,
				   nModulesPerSector, nChipsPerModule);
  }

  // Build sct barrel layers
  suffix = ""; // strip layers have identical services (so far)
  for (int i = 0; i < geoMgr.sctNumLayers(); i++) {
    DetType::Type type;
    if ( geoMgr.sctLayerType(i) == 1)  type = DetType::ShortStrip;
    else                               type = DetType::LongStrip;

    int modulesPerStave = geoMgr.sctModulesPerLadder(i)/2; // we need only half-stave modules for services

    tracker->constructBarrelLayer( geoMgr.sctLayerRadius(i), 0.5*geoMgr.sctLayerLength(i),
				   type, i, geoMgr.sctNumSectorsForLayer(i), suffix,
				   modulesPerStave, 4);
  }
  // SCT endcap
  for (int i = 0; i < geoMgr.sctNumDisks(); i++) {
    DetType::Type type = DetType::ShortStrip;  // FIXME: meaningless for a disk, but needed to satisfy interface

    int nModulesPerSector = 16;  // FIXME: hard-wired number!
    int nChipsPerModule = 1;     // FIXME: hard-wired number!

    tracker->constructEndcapLayer( geoMgr.sctDiskZ(i), geoMgr.sctInnerSupport(), 
				   geoMgr.sctDiskRMax(i), type, i,
				   geoMgr.sctEndcapNumSectorsForLayer(i), suffix,
				   nModulesPerSector, nChipsPerModule);
  }

  tracker->setGeoMgr(&geoMgr);
  return tracker;
}
