/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class MuonWall
// Class implementing geometry of the MuonWall 
// Author: franck.Martin<Franck.Martin@cern.ch>
// January 12, 2004.
//
//************************************************************

#include "MuonWall/MuonWallDescription.hh"
#include "MuonWall/MuonWall.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "G4Box.hh"
#include "G4ThreeVector.hh"
#include "FadsGeometry/DetectorFacilityBaseT.h"  
#include "FadsMaterial/MaterialManager.h"
#include "TileG4DetDescr/DetectorDescriptionStore.h"
#include <string>
#include <iostream>

static DetectorFacilityBaseT<MuonWall> muonWall("MuonWall"); 

MuonWall::MuonWall(G4String s) :
  DetectorFacility(s),
  muonwall(0)
{
}

MuonWall::~MuonWall()
{
}

void MuonWall::UseDescription(std::string dname)
{
  MuonWallDescription* i=dynamic_cast<MuonWallDescription*>(DetectorDescriptionStore::GetDetectorDescriptionStore()->GetDetectorDescription(dname));
  if (i)
    muonwall = i;
  else
    std::cout << "Warning. Description " << dname << " is not of the "
      "right type MuonWallDescription. Nothing done" << std::endl;
}

void MuonWall::BuildGeometry()
{
  FADS::MaterialManager* mmanager=FADS::MaterialManager::GetMaterialManager();
  // Obtain pointer to the sensitive detector

  /* G4String SDname = "MuonWallSD";
  MuonWallSD* muonwallSD = new MuonWallSD(SDname);
  G4SDManager* SDmanager = G4SDManager::GetSDMpointer();
  SDmanager->AddNewDetector(muonwallSD);
  */

  // MuonWall description : 

  double zScintillator = muonwall->zLength ;
  double yScintillator = muonwall->yLength ;
  double xScintillator = muonwall->xLength ;

  double dzmuonwall = 750.*CLHEP::mm;
  double dymuonwall = 425.*CLHEP::mm;
  double dxmuonwall = (xScintillator/2.)*CLHEP::mm;

  G4Box* wall = new G4Box("MuonWall",dxmuonwall,dymuonwall,dzmuonwall);

  G4LogicalVolume* wallV = new G4LogicalVolume(wall,mmanager->GetMaterial("Air"),"MuonWall");

  // ------- Create lead layers and place them inside the mother box --------
  double zLayer, yLayer, xLayer= 0. ;
  int nrOfLayers = 12;
    
  for (int j=0; j<nrOfLayers; j++)
  {
    
// Scintillator   
    G4Box* scintillatorLayer = new G4Box("ScintillatorLayer",xScintillator/2.,yScintillator/2.,zScintillator/2.);

    G4LogicalVolume* scintillatorLayerV = new G4LogicalVolume(scintillatorLayer,mmanager->GetMaterial("Scintillator"),"MuScintillatorLayer");

  // scintillatorLayerV->GetLogicalVolume()->SetSensitiveDetector(muonwallSD);

    if(j<6) {
     yLayer = yScintillator/2. + 25.;
     zLayer = -450. + (2*j+1)*zScintillator/2.;
    }
    else {
      yLayer = -yScintillator/2. + 25.;
      zLayer = -450. + (2*(j-6)+1)*zScintillator/2.;
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


  nrOfLayers = 3;
    
  for (int j=0; j<nrOfLayers; j++)
  {
    
// Scintillator : 500*100*20 instead of 400*200*20  
    G4Box* scintillatorLayer = new G4Box("SideScintiLayer",xScintillator/2.,(yScintillator+100.)/2.,(zScintillator-100.)/2.);

    G4LogicalVolume* scintillatorLayerV = new G4LogicalVolume(scintillatorLayer,mmanager->GetMaterial("Scintillator"),"SideMuScintiLayer");

    yLayer = -175. ;  // CLHEP::mm ?
     zLayer = -750. + (2*j+1)*(zScintillator-100.)/2.;

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
  envelope.theEnvelope=wallV;
}


