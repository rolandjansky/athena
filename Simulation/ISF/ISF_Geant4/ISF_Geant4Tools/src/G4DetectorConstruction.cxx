/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// G4DetectorConstruction.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "G4DetectorConstruction.h"

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// Geant4 includes
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "globals.hh"

G4DetectorConstruction::G4DetectorConstruction()
  :   m_worldLog(nullptr),
      m_worldPhys(nullptr)
{}


G4DetectorConstruction::~G4DetectorConstruction()
{}


void G4DetectorConstruction::dummyDetector()
{
  G4ThreeVector materialPosition(0.,0.,0.);

  // (1) WORLD
  // create the world setup
  G4Box* worldBox = new G4Box("WorldBox",25000.,25000.,25000.);
  
  // G4 material : vacuum setup
  G4Material* g4vacuum = G4Material::GetMaterial("Vacuum",false);
  if(!g4vacuum) g4vacuum = new G4Material("FatrasDummyVacuum",1.,1.01*CLHEP::g/CLHEP::mole,
                                           CLHEP::universe_mean_density,
                                           kStateGas,0.1*CLHEP::kelvin, 1.e-19*CLHEP::pascal);
  
  m_worldLog      = m_worldLog ? new(m_worldLog) G4LogicalVolume(worldBox,g4vacuum,"WorldLogical", 0, 0, 0) :
    new G4LogicalVolume(worldBox,g4vacuum,"WorldLogical", 0, 0, 0);
  m_worldPhys     = m_worldPhys ? new(m_worldPhys) G4PVPlacement(0,G4ThreeVector(0.,0.,0),"WorldPhysical",m_worldLog,0,false,0) :
    new G4PVPlacement(0,materialPosition,"WorldPhysical",m_worldLog,0,false,0);

  // (2) DETECTOR
  // create the detector setup, by definition 3 times the thickness
  //G4Box* detectordBox = new G4Box("WorldBox",10*thickness,10*thickness,10*thickness);
  //m_detectorLog      = m_detectorLog ? new(m_detectorLog) G4LogicalVolume(worldBox,s_g4vacuum,"DetectorLogical", 0, 0, 0) :
  //  new G4LogicalVolume(detectordBox,s_g4vacuum,"DetectordLogical", 0, 0, 0);
  //m_detectorPhys     = m_detectorPhys ? new(m_detectorPhys) G4PVPlacement(0,materialPosition,"DetectorPhysical",m_detectorLog,0,false,0) :
  //  new G4PVPlacement(0,materialPosition,"DetectorPhysical",m_detectorLog,m_worldPhys,false,0);

}


G4VPhysicalVolume* G4DetectorConstruction::Construct()
{
  // call the update detector method
  dummyDetector();
  return m_worldPhys;
}
