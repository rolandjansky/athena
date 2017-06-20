/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//class header
#include "PhantomBarrelTool.hh"

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


PhantomBarrelTool::PhantomBarrelTool(const std::string& type, const std::string& name, const IInterface* parent)
    : DetectorGeometryBase(type, name, parent),
    m_zLength(0),
    m_rMin(0),
    m_rMax(0)
{
  ATH_MSG_DEBUG( "PhantomBarrelTool constructor for " << name );
  declareProperty("ZLength", m_zLength, "");
  declareProperty("RMin", m_rMin, "");
  declareProperty("RMax", m_rMax, "");
}

PhantomBarrelTool::~PhantomBarrelTool() {}

void PhantomBarrelTool::BuildGeometry() {
  ATH_MSG_VERBOSE( name() << " PhantomBarrelTool::BuildGeometry(): Starting" );

  const double dxBarrel = (m_rMax - m_rMin) / 2. ;
  const double dyBarrel = m_zLength / 2. ;
  const double dzBarrel = m_zLength / 2. ;
  const double thicknessLayerIronEnvelop = 0.8 * CLHEP::mm;
  const double thicknessLayerAluEnvelop = 10. * CLHEP::mm;
  const double dyBarrelwithEnvelop = dyBarrel + thicknessLayerIronEnvelop + thicknessLayerAluEnvelop;
  const double dzBarrelwithEnvelop = dzBarrel + thicknessLayerIronEnvelop + thicknessLayerAluEnvelop;

  G4Box *barrel = new G4Box("PhantomBarrel", dxBarrel, dyBarrelwithEnvelop, dzBarrelwithEnvelop);

  // Get the materials
  const DataHandle<StoredMaterialManager> materialManager;
  if (StatusCode::SUCCESS != detStore()->retrieve(materialManager, std::string("MATERIALS"))) {
    //FIXME should probably at least print an ERROR here...
    return ;
  }
  const GeoMaterial *geoAir = materialManager->getMaterial("tile::Air");
  const GeoMaterial *geoGlue = materialManager->getMaterial("tile::Glue");
  const GeoMaterial *geoLead = materialManager->getMaterial("std::Lead");
  const GeoMaterial *geoIron = materialManager->getMaterial("std::Iron");
  const GeoMaterial *geoAluminium = materialManager->getMaterial("std::Aluminium");
  const GeoMaterial *geoScintillator = materialManager->getMaterial("tile::Scintillator");
  Geo2G4MaterialFactory theMaterialFactory;
  G4Material *airMaterial = theMaterialFactory.Build(geoAir);
  G4Material *glueMaterial = theMaterialFactory.Build(geoGlue);
  G4Material *leadMaterial = theMaterialFactory.Build(geoLead);
  G4Material *ironMaterial = theMaterialFactory.Build(geoIron);
  G4Material *aluminiumMaterial = theMaterialFactory.Build(geoAluminium);
  G4Material *scintillatorMaterial = theMaterialFactory.Build(geoScintillator);

  G4LogicalVolume *barrelV = new G4LogicalVolume(barrel, airMaterial, "PhantomBarrel");

  // ------- Create lead layers and place them inside the mother box --------
  double rCenterLayerLead;
  double rCenterLayerIron, rCenterLayerGlue, rCenterLayerIronEnvelop, rCenterLayerAluEnvelop;
  double rCenterLayerScintillator;

  // Determine the thickness of the lead layers
  const double thicknessLayerLead = 2. * CLHEP::mm ;
  const double thicknessLayerIron = 0.8 * CLHEP::mm ;
  const double thicknessLayerGlue = 0.2 * CLHEP::mm ;
  const double thicknessLayerScintillator = 6. * CLHEP::mm ;
  const double thicknessAir = 1. * CLHEP::mm ;
  double xLayers;
  const double nrOfLayers = 54. ;


  // place here 1. cm of alu + 0.8 mm layer for the front end plate
  rCenterLayerAluEnvelop = thicknessLayerAluEnvelop / 2.;
  rCenterLayerIronEnvelop = thicknessLayerAluEnvelop + thicknessLayerIronEnvelop / 2.;

  G4Box *aluLayerFE = new G4Box("AluLayerFE", thicknessLayerAluEnvelop / 2., dyBarrel, dzBarrel);
  G4LogicalVolume *aluLayerVFE = new G4LogicalVolume(aluLayerFE, aluminiumMaterial, "AluLayerFE");

  // aluLayerVFE->GetLogicalVolume()->SetSensitiveDetector(phantombarrelSD);
  xLayers = -dxBarrel + rCenterLayerAluEnvelop;

  G4PVPlacement *aluLayerVFEPhys __attribute__((unused)) =
    new G4PVPlacement(0,
                      G4ThreeVector(xLayers, 0., 0.),
                      aluLayerVFE,
                      "AluLayerFE" ,
                      barrelV,
                      false,
                      1);

  G4Box *ironLayerFE = new G4Box("IronLayerFE", thicknessLayerIronEnvelop / 2., dyBarrel, dzBarrel);
  G4LogicalVolume *ironLayerVFE = new G4LogicalVolume(ironLayerFE, ironMaterial, "IronLayerFE");

  // ironLayerVFE->GetLogicalVolume()->SetSensitiveDetector(phantombarrelSD);
  xLayers = -dxBarrel + rCenterLayerIronEnvelop;

  G4PVPlacement *ironLayerVFEPhys __attribute__((unused)) =
    new G4PVPlacement(0,
                      G4ThreeVector(xLayers, 0., 0.),
                      ironLayerVFE,
                      "IronLayerFE" ,
                      barrelV,
                      false,
                      1);




  for (int j = 0; j < nrOfLayers; j++) {

    rCenterLayerLead = (j + 1) * thicknessAir
        + j * (thicknessLayerIron + thicknessLayerScintillator + thicknessLayerGlue)
        + (2 * j + 1) * thicknessLayerLead / 2.;

    rCenterLayerIron = (j + 1) * (thicknessAir + thicknessLayerLead) + (2 * j + 1) * thicknessLayerIron / 2.
                       + j * (thicknessLayerScintillator + thicknessLayerGlue);

    rCenterLayerGlue = (j + 1) * (thicknessAir + thicknessLayerLead + thicknessLayerIron)
        + j * thicknessLayerScintillator + (2 * j + 1) * thicknessLayerGlue / 2.;

    rCenterLayerScintillator = (j + 1) * (thicknessAir + thicknessLayerLead + thicknessLayerIron + thicknessLayerGlue)
        + (2 * j + 1) * thicknessLayerScintillator / 2.;

    // lead layer
    G4Box *leadLayer = new G4Box("LeadLayer", thicknessLayerLead / 2., dyBarrel, dzBarrel);

    G4LogicalVolume *leadLayerV = new G4LogicalVolume(leadLayer, leadMaterial, "LeadLayer");

    // leadLayerV->GetLogicalVolume()->SetSensitiveDetector(phantombarrelSD);
    xLayers = -dxBarrel + rCenterLayerIronEnvelop + rCenterLayerLead;

    G4PVPlacement *leadLayerVPhys __attribute__((unused)) =
      new G4PVPlacement(0,
                        G4ThreeVector(xLayers, 0., 0.),
                        leadLayerV,
                        "LeadLayer",
                        barrelV,
                        false,
                        j + 1);

    // iron layer
    G4Box *ironLayer = new G4Box("IronLayer", thicknessLayerIron / 2., dyBarrel, dzBarrel);
    G4LogicalVolume *ironLayerV = new G4LogicalVolume(ironLayer, ironMaterial, "IronLayer");

    // ironLayerV->GetLogicalVolume()->SetSensitiveDetector(phantombarrelSD);
    xLayers = -dxBarrel + rCenterLayerIronEnvelop + rCenterLayerIron;

    G4PVPlacement *ironLayerVPhys __attribute__((unused)) =
      new G4PVPlacement(0,
                        G4ThreeVector(xLayers, 0., 0.),
                        ironLayerV,
                        "IronLayer",
                        barrelV,
                        false,
                        j + 1);

    // scintillator layer : scintillator size 374*374 mm2.
    double dyScintillator = dyBarrel - 13.;  // CLHEP::mm ?
    double dzScintillator = dzBarrel - 13.;

    G4Box *scintillatorLayer = new G4Box("ScintillatorLayer", thicknessLayerScintillator / 2., dyScintillator, dzScintillator);
    G4LogicalVolume *scintillatorLayerV = new G4LogicalVolume(scintillatorLayer, scintillatorMaterial, "ScintillatorLayer");

    // scintillatorLayerV->GetLogicalVolume()->SetSensitiveDetector(phantombarrelSD);
    xLayers = -dxBarrel + rCenterLayerIronEnvelop + rCenterLayerScintillator;

    G4PVPlacement *scintillatorLayerVPhys __attribute__((unused)) =
      new G4PVPlacement(0,
                        G4ThreeVector(xLayers, 0., 0.),
                        scintillatorLayerV,
                        "ScintillatorLayer",
                        barrelV,
                        false,
                        j + 1);

    // glue layer
    G4Box *glueLayer = new G4Box("GlueLayer", thicknessLayerGlue / 2., dyScintillator, dzScintillator);
    G4LogicalVolume *glueLayerV = new G4LogicalVolume(glueLayer, glueMaterial, "GlueLayer");

    // glueLayerV->GetLogicalVolume()->SetSensitiveDetector(phantombarrelSD);
    xLayers = -dxBarrel + rCenterLayerIronEnvelop + rCenterLayerGlue;

    G4PVPlacement *glueLayerVPhys __attribute__((unused)) =
      new G4PVPlacement(0,
                        G4ThreeVector(xLayers, 0., 0.),
                        glueLayerV,
                        "GlueLayer",
                        barrelV,
                        false,
                        j + 1);
  }

  // place here 1. cm Al + 0.8 mm Iron  for the back end plate
  G4Box *ironLayerBE = new G4Box("IronLayerBE", thicknessLayerIronEnvelop / 2., dyBarrel, dzBarrel);
  G4LogicalVolume *ironLayerVBE = new G4LogicalVolume(ironLayerBE, ironMaterial, "IronLayerBE");
  // ironLayerVBE->GetLogicalVolume()->SetSensitiveDetector(phantombarrelSD);
  xLayers = dxBarrel - rCenterLayerIronEnvelop;
  G4PVPlacement *ironLayerVBEPhys __attribute__((unused)) =
    new G4PVPlacement(0,
                      G4ThreeVector(xLayers, 0., 0.),
                      ironLayerVBE,
                      "IronLayerBE" ,
                      barrelV,
                      false,
                      1);

  G4Box *aluLayerBE = new G4Box("AluLayerBE", thicknessLayerAluEnvelop / 2., dyBarrel, dzBarrel);
  G4LogicalVolume *aluLayerVBE = new G4LogicalVolume(aluLayerBE, aluminiumMaterial, "AluLayerBE");

  // aluLayerVBE->GetLogicalVolume()->SetSensitiveDetector(phantombarrelSD);
  xLayers = dxBarrel - rCenterLayerAluEnvelop;

  G4PVPlacement *aluLayerVBEPhys __attribute__((unused)) =
    new G4PVPlacement(0,
                      G4ThreeVector(xLayers, 0., 0.),
                      aluLayerVBE,
                      "AluLayerBE" ,
                      barrelV,
                      false,
                      1);




  // place here rest of the envelopp : Top and Bottom sides
  for (int j = 0; j < 2;j++) {
    double yLayers = 0.0, zLayers = 0.0;
    G4Box *ironLayerTB = new G4Box("IronLayerTB",
                                   dxBarrel,
                                   thicknessLayerIronEnvelop / 2.,
                                   dzBarrel + thicknessLayerIronEnvelop + thicknessLayerAluEnvelop);
    G4LogicalVolume *ironLayerVTB = new G4LogicalVolume(ironLayerTB, ironMaterial, "IronLayerTB");
    // ironLayerVTB->GetLogicalVolume()->SetSensitiveDetector(phantombarrelSD);
    if (j == 0)
      yLayers = dyBarrel + thicknessLayerIronEnvelop / 2.;
    if (j == 1)
      yLayers = -dyBarrel - thicknessLayerIronEnvelop / 2.;
    G4PVPlacement *ironLayerVTBPhys __attribute__((unused)) =
      new G4PVPlacement(0,
                        G4ThreeVector(0., yLayers, 0.),
                        ironLayerVTB,
                        "IronLayerTB" ,
                        barrelV,
                        false,
                        j + 1);

    G4Box *aluLayerTB = new G4Box("AluLayerTB",
                                  dxBarrel,
                                  thicknessLayerAluEnvelop / 2.,
                                  dzBarrel + thicknessLayerIronEnvelop + thicknessLayerAluEnvelop);

    G4LogicalVolume *aluLayerVTB = new G4LogicalVolume(aluLayerTB, aluminiumMaterial, "AluLayerTB");
    // aluLayerVTB->GetLogicalVolume()->SetSensitiveDetector(phantombarrelSD);
    if (j == 0)
      yLayers = dyBarrel + thicknessLayerIronEnvelop + thicknessLayerAluEnvelop / 2.;
    if (j == 1)
      yLayers = -dyBarrel - thicknessLayerIronEnvelop - thicknessLayerAluEnvelop / 2.;

    G4PVPlacement *aluLayerVTBPhys __attribute__((unused)) =
      new G4PVPlacement(0,
                        G4ThreeVector(0., yLayers, 0.),
                        aluLayerVTB,
                        "AluLayerTB" ,
                        barrelV,
                        false,
                        j + 1);


    // Right and Left sides

    G4Box *ironLayerRL = new G4Box("IronLayerRL", dxBarrel, dyBarrel, thicknessLayerIronEnvelop / 2.);
    G4LogicalVolume *ironLayerVRL = new G4LogicalVolume(ironLayerRL, ironMaterial, "IronLayerRL");
    // ironLayerVRL->GetLogicalVolume()->SetSensitiveDetector(phantombarrelSD);
    if (j == 0)
      zLayers = dzBarrel + thicknessLayerIronEnvelop / 2.;
    if (j == 1)
      zLayers = -dzBarrel - thicknessLayerIronEnvelop / 2.;

    G4PVPlacement *ironLayerVRLPhys __attribute__((unused)) =
      new G4PVPlacement(0,
                        G4ThreeVector(0., 0., zLayers),
                        ironLayerVRL,
                        "IronLayerRL" ,
                        barrelV,
                        false,
                        j + 1);

    G4Box *aluLayerRL = new G4Box("AluLayerRL", dxBarrel, dyBarrel, thicknessLayerAluEnvelop / 2.);
    G4LogicalVolume *aluLayerVRL = new G4LogicalVolume(aluLayerRL, aluminiumMaterial, "AluLayerRL");
    //  ironLayerVRL->GetLogicalVolume()->SetSensitiveDetector(phantombarrelSD);
    if (j == 0)
      zLayers = dzBarrel + thicknessLayerIronEnvelop + thicknessLayerAluEnvelop / 2.;
    if (j == 1)
      zLayers = -dzBarrel - thicknessLayerIronEnvelop - thicknessLayerAluEnvelop / 2.;
    G4PVPlacement *aluLayerVRLPhys __attribute__((unused)) =
      new G4PVPlacement(0,
                        G4ThreeVector(0., 0., zLayers),
                        aluLayerVRL,
                        "AluLayerRL" ,
                        barrelV,
                        false,
                        j + 1);

  }


  // FINAL STEP
  m_envelope.theEnvelope = barrelV;
  ATH_MSG_VERBOSE( name() << " PhantomBarrelTool::BuildGeometry(): Finished" );
}
