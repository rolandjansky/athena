/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class MuonWallTool
// Class implementing geometry of the MuonWall
//
//************************************************************

// class header
#include "MuonWallTool.h"

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
#include "G4ThreeVector.hh"

// CLHEP headers
#include "CLHEP/Units/SystemOfUnits.h"

// STL headers
#include <string>
#include <iostream>

MuonWallTool::MuonWallTool(const std::string& type, const std::string& name, const IInterface* parent)
  : DetectorGeometryBase(type,name,parent),
    m_zLength(0.),
    m_yLength(0.),
    m_xLength(0.)
{
  ATH_MSG_DEBUG( "MuonWallTool constructor for " << name );
  declareProperty("ZLength", m_zLength, "");
  declareProperty("YLength", m_yLength, "");
  declareProperty("XLength", m_xLength, "");
}

MuonWallTool::~MuonWallTool()
{
}

void MuonWallTool::BuildGeometry() {
  // MuonWall description :

  const double zScintillator = m_zLength;
  const double yScintillator = m_yLength;
  const double xScintillator = m_xLength;
  const double dzmuonwall = 750. * CLHEP::mm;
  const double dymuonwall = 425. * CLHEP::mm;
  const double dxmuonwall = (xScintillator / 2.) * CLHEP::mm;

  G4Box *wall = new G4Box("MuonWall", dxmuonwall, dymuonwall, dzmuonwall);

  // Get the materials
  const DataHandle<StoredMaterialManager> materialManager;
  if (StatusCode::SUCCESS != detStore()->retrieve(materialManager, std::string("MATERIALS"))) {
    //FIXME should probably at least print an ERROR here...
    return;
  }
  const GeoMaterial *geoAir = materialManager->getMaterial("tile::Air");
  const GeoMaterial *geoScintillator = materialManager->getMaterial("tile::Scintillator");
  Geo2G4MaterialFactory theMaterialFactory;
  G4Material *airMaterial = theMaterialFactory.Build(geoAir);
  G4Material *scintillatorMaterial = theMaterialFactory.Build(geoScintillator);

  G4LogicalVolume *wallV = new G4LogicalVolume(wall, airMaterial, "MuonWall");

  // ------- Create lead layers and place them inside the mother box --------
  double zLayer, yLayer, xLayer = 0.;
  const int nrOfLayers = 12;

  for (int j = 0; j < nrOfLayers; j++) {

    // Scintillator
    G4Box *scintillatorLayer = new G4Box("ScintillatorLayer", xScintillator / 2., yScintillator / 2.,
        zScintillator / 2.);

    G4LogicalVolume *scintillatorLayerV = new G4LogicalVolume(scintillatorLayer, scintillatorMaterial,
        "MuScintillatorLayer");

    // scintillatorLayerV->GetLogicalVolume()->SetSensitiveDetector(muonwallSD);

    if (j < 6) {
      yLayer = yScintillator / 2. + 25.;
      zLayer = -450. + (2 * j + 1) * zScintillator / 2.;
    } else {
      yLayer = -yScintillator / 2. + 25.;
      zLayer = -450. + (2 * (j - 6) + 1) * zScintillator / 2.;
    }

    G4PVPlacement* scintillatorLayerVPhys __attribute__((unused)) =
        new G4PVPlacement(0,
                          G4ThreeVector(xLayer,yLayer,zLayer),
                          scintillatorLayerV,
                          "MuScintillatorLayer",
                          wallV,
                          false,
                          j+1);
    }


  const int nScintLayers = 3;

  for (int j = 0; j < nScintLayers; j++) {

    // Scintillator : 500*100*20 instead of 400*200*20
    G4Box* scintillatorLayer = new G4Box("SideScintiLayer", xScintillator / 2., (yScintillator + 100.) / 2.,
        (zScintillator - 100.) / 2.);

    G4LogicalVolume* scintillatorLayerV = new G4LogicalVolume(scintillatorLayer, scintillatorMaterial,
        "SideMuScintiLayer");

    yLayer = -175.;  // CLHEP::mm ?
    zLayer = -750. + (2 * j + 1) * (zScintillator - 100.) / 2.;

    G4PVPlacement* scintillatorLayerVPhys __attribute__((unused)) =
        new G4PVPlacement(0,
                          G4ThreeVector(xLayer,yLayer,zLayer),
                          scintillatorLayerV,
                          "SideMuScintiLayer",
                          wallV,
                          false,
                          j+1);
    }



  // FINAL STEP
  m_envelope.theEnvelope = wallV;
}
