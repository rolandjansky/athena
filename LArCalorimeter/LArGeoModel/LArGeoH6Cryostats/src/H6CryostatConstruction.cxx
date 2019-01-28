/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// H6CryostatConstruction

// Feb-2006 JPA; 
// 

#include "LArGeoH6Cryostats/H6CryostatConstruction.h"


#include "GeoModelKernel/GeoElement.h"  
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoFullPhysVol.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoVPhysVol.h"  
#include "GeoModelKernel/GeoVFullPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoTransform.h"  
#include "GeoModelKernel/GeoAlignableTransform.h"  
#include "GeoModelKernel/GeoIdentifierTag.h"  
#include "GeoModelKernel/GeoDefinitions.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"

// For the database:

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <string>
#include <cmath>

LArGeo::H6CryostatConstruction::H6CryostatConstruction():
  m_cryoMotherPhysical(NULL),
  m_cryoLArPhys(0)
{}

LArGeo::H6CryostatConstruction::~H6CryostatConstruction() {}

GeoVFullPhysVol* LArGeo::H6CryostatConstruction::GetEnvelope()
{

  if (m_cryoMotherPhysical) return m_cryoMotherPhysical;

  // Detector Store
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  StoreGateSvc *detStore;
  if (svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in H6CryostatConstruction, cannot access DetectorStore");
  }

  // Material Manager
  const StoredMaterialManager* materialManager = nullptr;
  if (StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) return 0;

  const GeoMaterial *Air  = materialManager->getMaterial("std::Air");
  if (!Air) {
    throw std::runtime_error("Error in H6CryostatConstruction, std::Air is not found.");
  }
  const GeoMaterial *Iron  = materialManager->getMaterial("std::Iron");
  if (!Iron) {
    throw std::runtime_error("Error in H6CryostatConstruction, std::Iron is not found.");
  }
  const GeoMaterial *LAr  = materialManager->getMaterial("std::LiquidArgon");
  if (!LAr) {
    throw std::runtime_error("Error in H6CryostatConstruction, std::LiquidArgon is not found.");
  }

  // GeoModelSvc
  ServiceHandle<IGeoModelSvc> geoModelSvc ("GeoModelSvc", "H6CryostatConstruction");
  if (geoModelSvc.retrieve().isFailure()) {
    throw std::runtime_error ("Cannot locate GeoModelSvc!!");
  }
  
  std::string AtlasVersion = geoModelSvc->atlasVersion();
  std::string LArVersion = geoModelSvc->LAr_VersionOverride();

  std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
  std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";

  //--------
  // Cryostat 

  // Here we should construct some sort of a cryostat:
  // First attempt at creating the H6 cryostat: 
  // (values taken from  HECCommonDetectorParamDef)
  //
  // A cylinder of half-height:   zcryo = 2000.0 *Gaudi::Units::mm
  // outer radius of warm-wall:   rwarm = 1295.5 *Gaudi::Units::mm
  // outer radius of vacuum:      rvac  = 1293.0 *Gaudi::Units::mm
  // outer radius of cold wall:   rcold = 1258.0 *Gaudi::Units::mm
  // outer radius of LAr:         rlar  = 1255.0 *Gaudi::Units::mm

  // needs to go into database: ---  4databa
  double  zcryo = 2000.0 *Gaudi::Units::mm;
  double  rwarm = 1295.5 *Gaudi::Units::mm;
  double  rvac  = 1293.0 *Gaudi::Units::mm;
  double  rcold = 1258.0 *Gaudi::Units::mm;
  double  rlar  = 1255.0 *Gaudi::Units::mm;

  std::string cryoMotherName = "LAr::H6::Cryostat::MotherVolume";

  // mother volume; cylinder of radius rwarm = outside of the cryostat warm wall 
  GeoTube* cryoMotherShape = new GeoTube(0.0 , rwarm+10.0, zcryo+10.0*Gaudi::Units::mm);  //  mother is a little bigger than warm wall   

  const GeoLogVol* cryoMotherLogical = new GeoLogVol(cryoMotherName, cryoMotherShape, Air);
  m_cryoMotherPhysical = new GeoFullPhysVol(cryoMotherLogical);

  // Now we have a physical air-filled cylindrical mother, into which we can place all we want and need.

  // First insert the Cryostat walls and the LAr - all simple cylinders
  //
  // 4databa ???
  std::string baseName          = "LArTB::H6::Cryostat::" ;
  std::string cryoWallName      = "LArTB::H6::Cryostat" ;
  std::string cryoWarmWallName  = baseName + "WarmWall" ;
  std::string cryoVacuumGapName = baseName + "Vacuum" ;
  std::string cryoColdWallName  = baseName + "ColdWall" ;
  std::string cryoLArName       = baseName + "LAr" ;     // <--- used to be called "Inside"
 

  // Warm Wall 
  GeoTube* cryoWarmWallShape = new GeoTube(0. , rwarm, zcryo);   
  const GeoLogVol* cryoWarmWallLog = new GeoLogVol(cryoWarmWallName, cryoWarmWallShape, Iron);
  //m_cryoMotherPhysical->add(new GeoNameTag(std::string("Cryostat")));    
  GeoPhysVol*  cryoWarmWallPhys = new GeoPhysVol(cryoWarmWallLog);
  m_cryoMotherPhysical->add(cryoWarmWallPhys);

  // "Vacuum" gap (filled with air...)
  GeoTube* cryoVacuumGapShape = new GeoTube(0. , rvac, zcryo-2.0*Gaudi::Units::mm);   // an arbitrary 2mm shorter to avoid confilct  
  const GeoLogVol* cryoVacuumGapLog = new GeoLogVol(cryoVacuumGapName, cryoVacuumGapShape, Air);
  GeoPhysVol*  cryoVacuumGapPhys = new GeoPhysVol(cryoVacuumGapLog);
  cryoWarmWallPhys->add(cryoVacuumGapPhys);

  // Cold Wall
  GeoTube* cryoColdWallShape = new GeoTube(0. , rcold, zcryo-4.0*Gaudi::Units::mm);  // an arbitrary 4mm shorter to avoid confilct   
  const GeoLogVol* cryoColdWallLog = new GeoLogVol(cryoColdWallName, cryoColdWallShape, Iron);
  GeoPhysVol*  cryoColdWallPhys = new GeoPhysVol(cryoColdWallLog);
  cryoVacuumGapPhys->add(cryoColdWallPhys);

 

  // At the moment the CryostatMother is LAr, so the FCal should be in there,
  // what would be better would be a mother of Air and LAr separately inside 
  // And the FCal the embedded in the LAr instead of the cryoMother!

  // Liquid Argon  
  GeoTube* cryoLArShape = new GeoTube(0. , rlar, zcryo-6.0*Gaudi::Units::mm);  // an arbitrary 2mm shorter to avoid confilct   
  const GeoLogVol* cryoLArLog = new GeoLogVol(cryoLArName, cryoLArShape, LAr);
  m_cryoMotherPhysical->add(new GeoNameTag(std::string("Cryostat LAr Physical")));    
  // m_cryoLArPhys is a class member so that we can place Detectors inside. 
  m_cryoLArPhys = new GeoPhysVol(cryoLArLog);
  cryoColdWallPhys->add(m_cryoLArPhys);
 

  // What remains needs to be inserted:
  // - Rohacell  (Needs an entry in LArCalorimeter/LArGeoModel/LArGeoAlgs/src/LArMaterialManager.cxx)

  return m_cryoMotherPhysical;

}

// Added so that FCal can be inserted in LArDetectorConstruction.
GeoPhysVol* LArGeo::H6CryostatConstruction::GetLArPhysical()
{
  return m_cryoLArPhys;
}




