/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ExcluderConstruction

// Feb-2006 JPA; 
// 

#include "ExcluderConstruction.h"


#include "GeoModelKernel/GeoElement.h"  
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoFullPhysVol.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoVPhysVol.h"  
#include "GeoModelKernel/GeoVFullPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoTubs.h"  
#include "GeoModelKernel/GeoBox.h"  
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoTransform.h"  
#include "GeoModelKernel/GeoAlignableTransform.h"  
#include "GeoModelKernel/GeoIdentifierTag.h"  
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"

// For transforms:

#include "CLHEP/Geometry/Transform3D.h" 
// For units:
#include "CLHEP/Units/PhysicalConstants.h"

// For the database:

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include "GeoModelInterfaces/IGeoDbTagSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"

#include <string>
#include <cmath>

LArGeo::ExcluderConstruction::ExcluderConstruction()
{
}

LArGeo::ExcluderConstruction::~ExcluderConstruction() {}

//GeoVFullPhysVol* LArGeo::ExcluderConstruction::GetEnvelope()
GeoPhysVol* LArGeo::ExcluderConstruction::GetEnvelope()
{

  // Need to do the equivalent for excluder here:                   <<<================ 
  //  if (cryoMotherPhysical) return cryoMotherPhysical;

  // Detector Store
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  StoreGateSvc *detStore;
  if (svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in ExcluderConstruction, cannot access DetectorStore");
  }

  // Material Manager

  // Need to add Rohacell here!                                     <<<=============== 

  DataHandle<StoredMaterialManager> materialManager;
  if (StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) return 0;


  // (use Air for the moment....)                                  <<<================ 
  GeoMaterial *Air  = materialManager->getMaterial("std::Air");
  if (!Air) {
    throw std::runtime_error("Error in ExcluderConstruction, std::Air is not found.");
  }

  // Define Rohacell Foam. 
  // Rohacell foam has density: 0.11g/cm3
  std::string name;
  double density;
  GeoElement*  C=materialManager->getElement("Carbon");
  GeoElement*  H=materialManager->getElement("Hydrogen");
  GeoElement*  O=materialManager->getElement("Oxygen");
  GeoElement*  N=materialManager->getElement("Nitrogen");
  GeoMaterial* Rohacell = new GeoMaterial(name="Rohacell", density=0.11*CLHEP::g/CLHEP::cm3);
  Rohacell->add(C,0.6465);
  Rohacell->add(H,0.07836);
  Rohacell->add(O,0.19137);
  Rohacell->add(N,0.08377);
  Rohacell->lock();

  ServiceHandle<IGeoDbTagSvc> geoDbTagSvc ("GeoDbTagSvc", "WallsConstruction");
  if (geoDbTagSvc.retrieve().isFailure()) {
    throw std::runtime_error ("Cannot locate GeoDbTagSvc!!");
  }
  
  std::string AtlasVersion = geoDbTagSvc->atlasVersion();
  std::string LArVersion = geoDbTagSvc->LAr_VersionOverride();
  
  std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
  std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";

  //--------
  // Now build the actual Excluder.

  // It is a Union out of a GeoBox and a GeoTubs.
  // Box Dimensions:
  double   xbox  =  300.0 *CLHEP::mm;
  double   ybox  =  160.0 *CLHEP::mm;
  double   zbox  =  300.7 *CLHEP::mm;
  //
  // Tubs Dimensions:
  double   ztubs =  300.0 *CLHEP::mm;
  double  phitubs=   76.2 *CLHEP::deg;
  double  delphi =   27.6 *CLHEP::deg;
  double   rcold = 1249.5 *CLHEP::mm;
  double   rmin  = 1220.0 *CLHEP::mm;

  // The radius of the cryostat cold wall is: 1250 CLHEP::mm
  // Before we make the union, we have to shift the box in y (that actually along the beam axis)
  //        and there, positive y goes from the cryostat centre towards the beam window.

  std::string ExcluderName = "LAr::H6::Cryostat::Excluder";

  GeoBox* rohaBox   = new GeoBox(xbox, ybox, zbox);                      //  The rectangular part of the excluder
  const GeoShapeShift & rohaBoxShift = (*rohaBox << HepGeom::TranslateY3D(1062.85*CLHEP::mm) );
  GeoTubs* rohaTubs = new GeoTubs(rmin, rcold, ztubs, phitubs, delphi);  //  The round part of the excluder  

  // Combine the two parts to make one excluder of the correct shape:
  const GeoShapeUnion* Excluder = new GeoShapeUnion(&rohaBoxShift, rohaTubs);

  const GeoLogVol* LogExcluder = new GeoLogVol(ExcluderName, Excluder, Rohacell);  // <<<==== air should be rohacell !!!
  GeoPhysVol* PhysExcluder = new GeoPhysVol(LogExcluder);
  PhysExcluder->add( new GeoNameTag(ExcluderName) );

  return PhysExcluder;

}
