/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class CombinedScintillator
// Class implementing geometry of the CombinedScintillator 
// Author: Anna Lupi<anna.lupi@pi.infn.it>
// November 2004.
//
//************************************************************

#include "CombinedScintillator/CombinedScintillatorDescription.hh"
#include "CombinedScintillator/CombinedScintillator.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "FadsGeometry/DetectorFacilityBaseT.h"  
#include "FadsMaterial/MaterialManager.h"
#include "TileG4DetDescr/DetectorDescriptionStore.h"
#include <string>
#include <iostream>

static DetectorFacilityBaseT<CombinedScintillator> combinedScintillator("CombinedScintillator"); 

CombinedScintillator::CombinedScintillator(G4String s) :
  DetectorFacility(s),
  tsci(0)
{
}

CombinedScintillator::~CombinedScintillator()
{
}

void CombinedScintillator::UseDescription(std::string dname)
{
  CombinedScintillatorDescription* i=dynamic_cast<CombinedScintillatorDescription*>(DetectorDescriptionStore::GetDetectorDescriptionStore()->GetDetectorDescription(dname));
  if (i)
    tsci = i;
  else
    std::cout << "Warning. Description " << dname << " is not of the "
      "right type CombinedScintillatorDescription. Nothing done" << std::endl;
}


void CombinedScintillator::BuildGeometry()
{

  // Scintillator between LAr & Tile described as a sector of a cylinder 

  // Define starting point for phi angle
  double phi1 = tsci->phiNeg;
  // Define range in phi to be covered
  double dphi = tsci->phiPos - tsci->phiNeg;

  G4Tubs* sci_mother = new G4Tubs("CScintillatorLayer",tsci->rMin,tsci->rMax,tsci->dzSci,phi1*CLHEP::deg,dphi*CLHEP::deg);

  FADS::MaterialManager* mmanager=FADS::MaterialManager::GetMaterialManager();
  G4LogicalVolume* sciV = new G4LogicalVolume(sci_mother,mmanager->GetMaterial("Scintillator"),"CScintillatorLayer");
 
  envelope.theEnvelope=sciV;

}


