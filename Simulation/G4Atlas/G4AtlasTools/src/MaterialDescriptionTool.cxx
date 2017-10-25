/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Primary include
#include "MaterialDescriptionTool.h"

// Geant4 includes
#include "G4NistManager.hh"
#include "G4Element.hh"
#include "G4Material.hh"

//-----------------------------------------------------------------------------
// Implementation file for class : MaterialDescriptionTool
//
// 2016-08-16: Andrea Dell'Acqua
//-----------------------------------------------------------------------------


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MaterialDescriptionTool::MaterialDescriptionTool(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent)
  : GeometryConfigurationToolBase(type, name, parent)
{
}

StatusCode MaterialDescriptionTool::preGeometryConfigure() const
{
  ATH_MSG_DEBUG("MaterialDescriptionTool::preGeometryConfigure()");

  G4NistManager* man = G4NistManager::Instance();
  G4Element* N   = man->FindOrBuildElement("N");
  G4Element* O   = man->FindOrBuildElement("O");
  G4Element* Ar  = man->FindOrBuildElement("Ar");
  G4Element* H   = man->FindOrBuildElement("H");
  G4Element* Si  = man->FindOrBuildElement("Si");
  G4Element* Ca  = man->FindOrBuildElement("Ca");
  G4Element* Al  = man->FindOrBuildElement("Al");
  G4Element* Fe  = man->FindOrBuildElement("Fe");
  G4Element* C   = man->FindOrBuildElement("C");

  // Constructor of G4Material inserts it into a table for later use
  G4Material *air = new G4Material("Air", 0.001214*CLHEP::g/CLHEP::cm3,4);
  air->AddElement( N, 0.7494);
  air->AddElement( O, 0.2369);
  air->AddElement(Ar, 0.0129);
  air->AddElement( H, 0.0008);

  G4Material *vacuum = new G4Material("Vacuum", 0.000001*CLHEP::g/CLHEP::cm3,1);
  vacuum->AddMaterial(air, 1.0);

  G4Material *material_concrete = new G4Material("Concrete",2.3*CLHEP::g/CLHEP::cm3,6);
  material_concrete->AddElement(Si,0.227915);
  material_concrete->AddElement(O,0.605410);
  material_concrete->AddElement(H,0.099720);
  material_concrete->AddElement(Ca,0.049860);
  material_concrete->AddElement(Al,0.014245);
  material_concrete->AddElement(Fe,0.002850);

  G4Material *material_scintillator = new G4Material("Scintillator",1.032*CLHEP::g/CLHEP::cm3,2);
  material_scintillator->AddElement(C,0.5);
  material_scintillator->AddElement(H,0.5);

  G4Material *material_mylar = new G4Material("Mylar",1.39*CLHEP::g/CLHEP::cm3,3);
  material_mylar->AddElement(H,4./11.);
  material_mylar->AddElement(C,5./11.);
  material_mylar->AddElement(O,2./11.);

  G4Material *material_rock = new G4Material("Rock",2.33*CLHEP::g/CLHEP::cm3,1);
  material_rock->AddElement(Si,1);

  return StatusCode::SUCCESS;
}

StatusCode MaterialDescriptionTool::postGeometryConfigure() const
{
  ATH_MSG_DEBUG("MaterialDescriptionTool::postGeometryConfigure()");

  return StatusCode::SUCCESS;
}
