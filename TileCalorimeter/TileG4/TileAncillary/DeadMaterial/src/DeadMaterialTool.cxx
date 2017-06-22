/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//class header
#include "DeadMaterialTool.h"

//Athena headers
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoMaterial2G4/Geo2G4MaterialFactory.h"
#include "StoreGate/DataHandle.h"

// Geant4 headers
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"

// CLHEP headers
#include "CLHEP/Units/SystemOfUnits.h"

DeadMaterialTool::DeadMaterialTool(const std::string& type, const std::string& name, const IInterface* parent)
  : DetectorGeometryBase(type,name,parent),
    m_zLength(0),
    m_yLength(0),
    m_xLength(0)
{
  ATH_MSG_DEBUG( "DeadMaterialTool constructor for " << name );
  declareProperty("ZLength", m_zLength, "");
  declareProperty("YLength", m_yLength, "");
  declareProperty("XLength", m_xLength, "");
}

DeadMaterialTool::~DeadMaterialTool()
{

}

void DeadMaterialTool::BuildGeometry()
{
  ATH_MSG_VERBOSE( name() << " DeadMaterialTool::BuildGeometry(): Starting" );

  const double dzdeadmaterial = (m_zLength)/2.  *CLHEP::mm;
  const double dydeadmaterial = (m_yLength)/2.  *CLHEP::mm;
  const double dxdeadmaterial = (m_xLength)/2.  *CLHEP::mm;

  ATH_MSG_DEBUG ( "DeadMaterial geometry " <<  dzdeadmaterial << " " << dydeadmaterial << " " << dxdeadmaterial );

  G4Box *wall = new G4Box("DeadMaterial",dxdeadmaterial,dydeadmaterial,dzdeadmaterial);

  // Get the materials
  const DataHandle<StoredMaterialManager> materialManager;
  if (StatusCode::SUCCESS != detStore()->retrieve(materialManager, std::string("MATERIALS")))
    {
      //FIXME should probably at least print an ERROR here...
      return;
    }
  const GeoMaterial *geoAir          = materialManager->getMaterial("tile::Air");
  const GeoMaterial *geoIron         = materialManager->getMaterial("std::Iron");
  Geo2G4MaterialFactory theMaterialFactory;
  G4Material *airMaterial=theMaterialFactory.Build(geoAir);
  G4Material *ironMaterial=theMaterialFactory.Build(geoIron);

  G4LogicalVolume *wallV = new G4LogicalVolume(wall,airMaterial,"DeadMaterial");

  // ------- Create iron bar and place it inside the mother box --------
  const double IronWidth = 5./2. *CLHEP::mm;
  for (int j=0; j<2; j++)
    {
      // X Iron plates
      G4Box *ironLayerX = new G4Box("IronLayerX",IronWidth,dydeadmaterial,dzdeadmaterial);
      G4LogicalVolume *ironLayerXV = new G4LogicalVolume(ironLayerX,ironMaterial,"DMIronLayerX");

      double xLayer=0.0, zLayer=0.0;
      if(j==0) xLayer = -dxdeadmaterial+IronWidth;
      if(j==1) xLayer = dxdeadmaterial-IronWidth;
      G4PVPlacement *ironLayerXVPhys __attribute__((unused)) =
        new G4PVPlacement (0,
                           G4ThreeVector(xLayer,0,0),
                           ironLayerXV,
                           "DMIronLayerX",
                           wallV,
                           false,
                           j+1);

      // Z Iron plates
      const double IronWidthX = 140./2. *CLHEP::mm;
      G4Box *ironLayerZ = new G4Box("IronLayerZ",IronWidthX,dydeadmaterial,IronWidth);
      G4LogicalVolume *ironLayerZV = new G4LogicalVolume(ironLayerZ,ironMaterial,"DMIronLayerZ");
      if(j==0) zLayer = -dzdeadmaterial+IronWidth;
      if(j==1) zLayer = dzdeadmaterial-IronWidth;
      G4PVPlacement *ironLayerZVPhys __attribute__((unused)) =
        new G4PVPlacement (0,
                           G4ThreeVector(0,0,zLayer),
                           ironLayerZV,
                           "DMIronLayerZ",
                           wallV,
                           false,
                           j+1);
    }

  // FINAL STEP
  m_envelope.theEnvelope=wallV;
  ATH_MSG_VERBOSE( name() << " DeadMaterialTool::BuildGeometry(): Finished" );
}
