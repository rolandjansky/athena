/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class LArCrate
// Class implementing geometry of the LArCrate 
// Author: franck.Martin<Franck.Martin@cern.ch>
// November 19, 2004.
//
//************************************************************

#include "LarCrate/LArCrateDescription.hh"
#include "LarCrate/LArCrate.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "G4Box.hh"
#include "G4ThreeVector.hh"
#include "FadsMaterial/MaterialManager.h"
#include "FadsGeometry/DetectorFacilityBaseT.h"  
#include "TileG4DetDescr/DetectorDescriptionStore.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include <string>
#include <iostream>

static DetectorFacilityBaseT<LArCrate> larCrate("LArCrate"); 

LArCrate::LArCrate(G4String s) :
  DetectorFacility(s),
  m_larcrate(0),
  m_msgSvc(0)
{
}

LArCrate::~LArCrate()
{
}

void LArCrate::UseDescription(std::string dname)
{
  LArCrateDescription* i=dynamic_cast<LArCrateDescription*>(DetectorDescriptionStore::GetDetectorDescriptionStore()->GetDetectorDescription(dname));
  if (i)
    m_larcrate = i;
  else
    std::cout << "Warning. Description " << dname << " is not of the "
      "right type LArCrateDescription. Nothing done" << std::endl;
}

void LArCrate::BuildGeometry()
{  
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode status = svcLocator->service("MessageSvc", m_msgSvc);
  MsgStream log(m_msgSvc, "LArCrate");
  
  FADS::MaterialManager* mmanager=FADS::MaterialManager::GetMaterialManager();

  if(status.isFailure())
    log << MSG::FATAL << "MessageSvc not found!" << endreq;
  else
    log << MSG::DEBUG << "MessageSvc initialized" << endreq;

  double dzCrate = (m_larcrate->zLength)/2.*CLHEP::mm ;
  double dyCrate = (m_larcrate->yLength)/2.*CLHEP::mm ;
  double dxCrate = (m_larcrate->xLength)/2.*CLHEP::mm ;

  G4Box* emptybox = new G4Box("Crate",dxCrate,dyCrate,dzCrate);

  G4LogicalVolume* emptyboxV = new G4LogicalVolume(emptybox,mmanager->GetMaterial("LArCrateMaterial"),"LarCrate");
  
  // creates wall of 5mm aluminum thickness
  double thicknessAlu = 5.*CLHEP::mm ;

  for(int j=0; j<2;j++){
   double xLayers=0.0, yLayers=0.0, zLayers=0.0;

   // top and Bottom plates 
   G4Box* AluLayerTB = new G4Box("AluLayerTB",dxCrate,thicknessAlu/2.,dzCrate);
   G4LogicalVolume* AluLayerVTB = new G4LogicalVolume(AluLayerTB,mmanager->GetMaterial("Aluminum"),"AluLayerTB");
   if(j==0) yLayers = dyCrate - thicknessAlu/2.;
   if(j==1) yLayers = -dyCrate + thicknessAlu/2.; 
   G4PVPlacement* AluLayerVTBPhys __attribute__((unused)) =   
       new G4PVPlacement(0,
                         G4ThreeVector(0.,yLayers,0.),
						 AluLayerVTB,
                         "AluLayerTB" ,
                          emptyboxV,
						  false,
                          j+1);
 
    // Right and Left sides
     G4Box* AluLayerRL = new G4Box("AluLayerRL",dxCrate,dyCrate-thicknessAlu,thicknessAlu/2.);
     G4LogicalVolume* AluLayerVRL = new G4LogicalVolume(AluLayerRL,mmanager->GetMaterial("Aluminum"),"AluLayerRL");
     if(j==0) zLayers = dzCrate - thicknessAlu/2.;
     if(j==1) zLayers = -dzCrate + thicknessAlu/2.;    
	 G4PVPlacement* AluLayerVRLPhys __attribute__((unused)) =   
         new G4PVPlacement(0,
                           G4ThreeVector(0.,0.,zLayers),
						   AluLayerVRL,
                           "AluLayerRL" ,
                           emptyboxV,
						   false,
                           j+1); 
 
    // Front and back sides
     G4Box* AluLayerFB = new G4Box("AluLayerFB",thicknessAlu/2.,dyCrate-thicknessAlu,dzCrate-thicknessAlu);
     G4LogicalVolume* AluLayerVFB = new G4LogicalVolume(AluLayerFB,mmanager->GetMaterial("Aluminum"),"AluLayerFB");
     if(j==0) xLayers = dxCrate - thicknessAlu/2.;
     if(j==1) xLayers = -dxCrate + thicknessAlu/2.;    
	 G4PVPlacement* AluLayerVFBPhys __attribute__((unused)) =   
         new G4PVPlacement(0,
                           G4ThreeVector(xLayers,0.,0.),
						   AluLayerVFB,
                           "AluLayerFB" ,
                           emptyboxV,
						   false,
                           j+1);
     }
 
  // fills the Volume with LArCrateMaterial to be defined
  
  /*  FadsBox* LArCratebox = new FadsBox("InnerLArCrate",dxCrate-thicknessAlu,dyCrate-thicknessAlu,dzCrate-thicknessAlu);

  G4LogicalVolume* LArCrateboxV = new G4LogicalVolume(LArCratebox,mmanager->GetMaterial("TileAir"),"InnerLArCrateV"); 
  LArCrateboxV->Position(0,
  G4PVPlacement* LArCrateboxVPhys __attribute__((unused)) = 
                         G4ThreeVector(0.,0.,0.),
						 LArCrateboxV,
                         "InnerLArCrate" ,
                         emptyboxV,
						 false,
                         1);
  */

  envelope.theEnvelope=emptyboxV;
  log << MSG::DEBUG << "LAr Crate is builded " << endreq;

}


