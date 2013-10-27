/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class PhantomBarrel. 
// Class implementing geometry of the Phantom (Lead/Iron/Scintillator) 
// Calorimeter (Prague calorimeter used as forward calo. in combined test beam)
// Author: franck.Martin<Franck.Martin@cern.ch>
// December 15th, 2003.
//
//************************************************************

#include "PhantomCalorimeter/PhantomBarrelDescription.hh"
#include "PhantomCalorimeter/PhantomBarrel.hh"
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

static DetectorFacilityBaseT<PhantomBarrel> phantomBarrel("PhantomBarrel"); 

PhantomBarrel::PhantomBarrel(G4String s) :
  DetectorFacility(s),
  phantombar(0)
{
}

PhantomBarrel::~PhantomBarrel()
{
}

void PhantomBarrel::UseDescription(std::string dname)
{
  PhantomBarrelDescription* i=dynamic_cast<PhantomBarrelDescription*>(DetectorDescriptionStore::GetDetectorDescriptionStore()->GetDetectorDescription(dname));
  if (i)
    phantombar=i;
  else
    std::cout << "Warning. Description " << dname << " is not of the "
      "right type PhantomBarrelDescription. Nothing done" << std::endl;
}

void PhantomBarrel::BuildGeometry()
{
  FADS::MaterialManager* mman=FADS::MaterialManager::GetMaterialManager();

  // Obtain pointer to the sensitive detector
  /* G4String SDname = "PhantomBarrelSD";
  PhantomBarrelSD* phantombarrelSD = new PhantomBarrelSD(SDname);
  G4SDManager* SDmanager = G4SDManager::GetSDMpointer();
  SDmanager->AddNewDetector(phantombarrelSD);
  */

  // ----------------- Mother volume for the Phantom Barrel -------------------
  // The box with dimensions:
  //   ZLength="400."
  //   RMin="1100."
  //   RMax="1670."

  double dxBarrel = (phantombar->rMax - phantombar->rMin)/2. ;
  double dyBarrel = phantombar->zLength/2. ;
  double dzBarrel = phantombar->zLength/2. ;
  double thicknessLayerIronEnvelop = 0.8*CLHEP::mm;
  double thicknessLayerAluEnvelop = 10.*CLHEP::mm; 
  double dyBarrelwithEnvelop = dyBarrel+thicknessLayerIronEnvelop+thicknessLayerAluEnvelop;
  double dzBarrelwithEnvelop = dzBarrel+thicknessLayerIronEnvelop+thicknessLayerAluEnvelop; 

  G4Box* barrel = new G4Box("PhantomBarrel",dxBarrel,dyBarrelwithEnvelop,dzBarrelwithEnvelop);

  G4LogicalVolume* barrelV = new G4LogicalVolume(barrel,mman->GetMaterial("Air"),"PhantomBarrel");

  // ------- Create lead layers and place them inside the mother box --------
  double rCenterLayerLead;
  double rCenterLayerIron, rCenterLayerGlue,rCenterLayerIronEnvelop, rCenterLayerAluEnvelop;
  double rCenterLayerScintillator;

  // Determine the thickness of the lead layers
  double thicknessLayerLead = 2.*CLHEP::mm ;
  double thicknessLayerIron = 0.8*CLHEP::mm ;
  double thicknessLayerGlue = 0.2*CLHEP::mm ;
  double thicknessLayerScintillator = 6.*CLHEP::mm ;
  double thicknessAir = 1.*CLHEP::mm ;  
  double xLayers;
  double nrOfLayers = 54. ;


  // place here 1. cm of alu + 0.8 mm layer for the front end plate
    rCenterLayerAluEnvelop = thicknessLayerAluEnvelop/2.;
    rCenterLayerIronEnvelop = thicknessLayerAluEnvelop + thicknessLayerIronEnvelop/2.;
    
    G4Box* aluLayerFE = new G4Box("AluLayerFE",thicknessLayerAluEnvelop/2.,dyBarrel,dzBarrel);
    G4LogicalVolume* aluLayerVFE = new G4LogicalVolume(aluLayerFE,mman->GetMaterial("Aluminum"),"AluLayerFE");

    // aluLayerVFE->GetLogicalVolume()->SetSensitiveDetector(phantombarrelSD);
    xLayers = -dxBarrel + rCenterLayerAluEnvelop;

	G4PVPlacement* aluLayerVFEPhys __attribute__((unused)) =
	  new G4PVPlacement(0,
 		       G4ThreeVector(xLayers,0.,0.),
			   aluLayerVFE,
	 		   "AluLayerFE" ,
		 	   barrelV,
			   false,
			   1);

    G4Box* ironLayerFE = new G4Box("IronLayerFE",thicknessLayerIronEnvelop/2.,dyBarrel,dzBarrel);
    G4LogicalVolume* ironLayerVFE = new G4LogicalVolume(ironLayerFE,mman->GetMaterial("Iron"),"IronLayerFE");

    // ironLayerVFE->GetLogicalVolume()->SetSensitiveDetector(phantombarrelSD);
    xLayers = -dxBarrel + rCenterLayerIronEnvelop;

	G4PVPlacement* ironLayerVFEPhys __attribute__((unused)) =
	  new G4PVPlacement(0,
 		       G4ThreeVector(xLayers,0.,0.),
			   ironLayerVFE,
	 		   "IronLayerFE" ,
		 	   barrelV,
			   false,
			   1);
    


    
  for (int j=0; j<nrOfLayers; j++)
  {
    rCenterLayerLead = (j+1)*thicknessAir + j*(thicknessLayerIron + thicknessLayerScintillator + thicknessLayerGlue) + (2*j+1)*thicknessLayerLead/2.;
    rCenterLayerIron = (j+1)*(thicknessAir+thicknessLayerLead) + (2*j+1)*thicknessLayerIron/2. + j*(thicknessLayerScintillator+ thicknessLayerGlue);
    rCenterLayerGlue = (j+1)*(thicknessAir+thicknessLayerLead + thicknessLayerIron) + j*thicknessLayerScintillator + (2*j+1)*thicknessLayerGlue/2.; 
    rCenterLayerScintillator = (j+1)*(thicknessAir + thicknessLayerLead + thicknessLayerIron + thicknessLayerGlue) + (2*j+1)*thicknessLayerScintillator/2.;
    
// lead layer   
    G4Box* leadLayer = new G4Box("LeadLayer",thicknessLayerLead/2.,dyBarrel,dzBarrel);

    G4LogicalVolume* leadLayerV = new G4LogicalVolume(leadLayer,mman->GetMaterial("Lead"),"LeadLayer");

    // leadLayerV->GetLogicalVolume()->SetSensitiveDetector(phantombarrelSD);
    xLayers = -dxBarrel+rCenterLayerIronEnvelop+rCenterLayerLead;

	G4PVPlacement* leadLayerVPhys __attribute__((unused)) =
	  new G4PVPlacement(0,
			 G4ThreeVector(xLayers,0.,0.),
			 leadLayerV,
			 "LeadLayer",
			 barrelV,
			 false,
			 j+1);

// iron layer    
    G4Box* ironLayer = new G4Box("IronLayer",thicknessLayerIron/2.,dyBarrel,dzBarrel);
    G4LogicalVolume* ironLayerV = new G4LogicalVolume(ironLayer,mman->GetMaterial("Iron"),"IronLayer");

    // ironLayerV->GetLogicalVolume()->SetSensitiveDetector(phantombarrelSD);
    xLayers = -dxBarrel+rCenterLayerIronEnvelop+rCenterLayerIron;

	G4PVPlacement* ironLayerVPhys __attribute__((unused)) =
	  new G4PVPlacement(0,
			 G4ThreeVector(xLayers,0.,0.),
			 ironLayerV,
			 "IronLayer",
			 barrelV,
			 false,
			 j+1);

// scintillator layer : scintillator size 374*374 mm2.
    double dyScintillator = dyBarrel - 13.;  // CLHEP::mm ?
    double dzScintillator = dzBarrel - 13.;   

    G4Box* scintillatorLayer = new G4Box("ScintillatorLayer",thicknessLayerScintillator/2.,dyScintillator,dzScintillator);
    G4LogicalVolume* scintillatorLayerV = new G4LogicalVolume(scintillatorLayer,mman->GetMaterial("Scintillator"),"ScintillatorLayer");

    // scintillatorLayerV->GetLogicalVolume()->SetSensitiveDetector(phantombarrelSD);
    xLayers = -dxBarrel+rCenterLayerIronEnvelop+rCenterLayerScintillator;

	G4PVPlacement* scintillatorLayerVPhys __attribute__((unused)) =
	  new G4PVPlacement(0,
			 G4ThreeVector(xLayers,0.,0.),
			 scintillatorLayerV,
			 "ScintillatorLayer",
			 barrelV,
			 false,
			 j+1);

// glue layer
    G4Box* glueLayer = new G4Box("GlueLayer",thicknessLayerGlue/2.,dyScintillator,dzScintillator);
    G4LogicalVolume* glueLayerV = new G4LogicalVolume(glueLayer,mman->GetMaterial("Glue"),"GlueLayer");

    // glueLayerV->GetLogicalVolume()->SetSensitiveDetector(phantombarrelSD);
    xLayers = -dxBarrel+rCenterLayerIronEnvelop+rCenterLayerGlue;

	G4PVPlacement* glueLayerVPhys __attribute__((unused)) =
	  new G4PVPlacement(0,
		G4ThreeVector(xLayers,0.,0.),
		glueLayerV,
		"GlueLayer",
		barrelV,
		false,
 	 	j+1);
  }

  // place here 1. cm Al + 0.8 mm Iron  for the back end plate
    G4Box* ironLayerBE = new G4Box("IronLayerBE",thicknessLayerIronEnvelop/2.,dyBarrel,dzBarrel);
    G4LogicalVolume* ironLayerVBE = new G4LogicalVolume(ironLayerBE,mman->GetMaterial("Iron"),"IronLayerBE");
    // ironLayerVBE->GetLogicalVolume()->SetSensitiveDetector(phantombarrelSD);
    xLayers = dxBarrel - rCenterLayerIronEnvelop;
	G4PVPlacement* ironLayerVBEPhys __attribute__((unused)) =
	  new G4PVPlacement(0,
 		       G4ThreeVector(xLayers,0.,0.),
			   ironLayerVBE,
	 		   "IronLayerBE" ,
		 	   barrelV,
			   false,
			   1);

    G4Box* aluLayerBE = new G4Box("AluLayerBE",thicknessLayerAluEnvelop/2.,dyBarrel,dzBarrel);
    G4LogicalVolume* aluLayerVBE = new G4LogicalVolume(aluLayerBE,mman->GetMaterial("Aluminum"),"AluLayerBE");

    // aluLayerVBE->GetLogicalVolume()->SetSensitiveDetector(phantombarrelSD);
    xLayers = dxBarrel - rCenterLayerAluEnvelop;

	G4PVPlacement* aluLayerVBEPhys __attribute__((unused)) =
	  new G4PVPlacement(0,
 		       G4ThreeVector(xLayers,0.,0.),
			   aluLayerVBE,
	 		   "AluLayerBE" ,
		 	   barrelV,
			   false,
			   1);
    



  // place here rest of the envelopp : Top and Bottom sides 
    for(int j=0; j<2;j++)
     {
     double yLayers=0.0, zLayers=0.0;
     G4Box* ironLayerTB = new G4Box("IronLayerTB",dxBarrel,thicknessLayerIronEnvelop/2.,dzBarrel+thicknessLayerIronEnvelop+thicknessLayerAluEnvelop);
     G4LogicalVolume* ironLayerVTB = new G4LogicalVolume(ironLayerTB,mman->GetMaterial("Iron"),"IronLayerTB");
     // ironLayerVTB->GetLogicalVolume()->SetSensitiveDetector(phantombarrelSD);
     if(j==0)yLayers = dyBarrel + thicknessLayerIronEnvelop/2.;
     if(j==1)yLayers = -dyBarrel - thicknessLayerIronEnvelop/2.;    
	 G4PVPlacement* ironLayerVTBPhys __attribute__((unused)) =
	  new G4PVPlacement(0,
 	 	       G4ThreeVector(0.,yLayers,0.),
			   ironLayerVTB,
	 		   "IronLayerTB" ,
		 	   barrelV,
			   false,
			   j+1); 

     G4Box* aluLayerTB = new G4Box("AluLayerTB",dxBarrel,thicknessLayerAluEnvelop/2.,dzBarrel+thicknessLayerIronEnvelop+thicknessLayerAluEnvelop);
     G4LogicalVolume* aluLayerVTB = new G4LogicalVolume(aluLayerTB,mman->GetMaterial("Aluminum"),"AluLayerTB");
     // aluLayerVTB->GetLogicalVolume()->SetSensitiveDetector(phantombarrelSD);
     if(j==0)yLayers = dyBarrel + thicknessLayerIronEnvelop +thicknessLayerAluEnvelop/2.;
     if(j==1)yLayers = -dyBarrel - thicknessLayerIronEnvelop - thicknessLayerAluEnvelop/2.;    
     
	 G4PVPlacement* aluLayerVTBPhys __attribute__((unused)) =
	  new G4PVPlacement(0,
 	 	       G4ThreeVector(0.,yLayers,0.),
			   aluLayerVTB,
	 		   "AluLayerTB" ,
		 	   barrelV,
			   false,
			   j+1); 


    // Right and Left sides

     G4Box* ironLayerRL = new G4Box("IronLayerRL",dxBarrel,dyBarrel,thicknessLayerIronEnvelop/2.);
     G4LogicalVolume* ironLayerVRL = new G4LogicalVolume(ironLayerRL,mman->GetMaterial("Iron"),"IronLayerRL");
     // ironLayerVRL->GetLogicalVolume()->SetSensitiveDetector(phantombarrelSD);
     if(j==0) zLayers = dzBarrel + thicknessLayerIronEnvelop/2.;
     if(j==1) zLayers = -dzBarrel - thicknessLayerIronEnvelop/2.;    

	 G4PVPlacement* ironLayerVRLPhys __attribute__((unused)) =
	  new G4PVPlacement(0,
 	 	       G4ThreeVector(0.,0.,zLayers),
			   ironLayerVRL,
	 		   "IronLayerRL" ,
		 	   barrelV,
			   false,
			   j+1); 

     G4Box* aluLayerRL = new G4Box("AluLayerRL",dxBarrel,dyBarrel,thicknessLayerAluEnvelop/2.);
     G4LogicalVolume* aluLayerVRL = new G4LogicalVolume(aluLayerRL,mman->GetMaterial("Aluminum"),"AluLayerRL");
     //  ironLayerVRL->GetLogicalVolume()->SetSensitiveDetector(phantombarrelSD);
     if(j==0) zLayers = dzBarrel + thicknessLayerIronEnvelop + thicknessLayerAluEnvelop/2.;
     if(j==1) zLayers = -dzBarrel - thicknessLayerIronEnvelop - thicknessLayerAluEnvelop/2.;    
	 G4PVPlacement* aluLayerVRLPhys __attribute__((unused)) =
	  new G4PVPlacement(0,
 	 	       G4ThreeVector(0.,0.,zLayers),
			   aluLayerVRL,
	 		   "AluLayerRL" ,
		 	   barrelV,
			   false,
			   j+1); 

     }


  // FINAL STEP
  envelope.theEnvelope=barrelV;
}


