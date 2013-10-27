/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class DeadMaterial
// Class implementing geometry of the LAr cryostat support+LAr crates 
// Author:franck.Martin<Franck.Martin@cern.ch>, anna.Lupi<anna.lupi@pi.infn.it>
// July 12, 2004.
//
//************************************************************

#include "DeadMaterial/DeadMaterialDescription.hh"
#include "DeadMaterial/DeadMaterial.hh"
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

static DetectorFacilityBaseT<DeadMaterial> deadMaterial("DeadMaterial"); 

DeadMaterial::DeadMaterial(G4String s) :
  DetectorFacility(s),
  m_deadmaterial(0),
  m_msgSvc(0)
{
}

DeadMaterial::~DeadMaterial()
{
}

void DeadMaterial::UseDescription(std::string dname)
{
  DeadMaterialDescription* i=dynamic_cast<DeadMaterialDescription*>(DetectorDescriptionStore::GetDetectorDescriptionStore()->GetDetectorDescription(dname));
  //  DeadMaterialDescription* i=dynamic_cast<DeadMaterialDescription*>(GetDescription(dname));
  if (i)
    m_deadmaterial = i;
  else
    std::cout << "Warning. Description " << dname << " is not of the "
      "right type DeadMaterialDescription. Nothing done" << std::endl;
}

void DeadMaterial::BuildGeometry()
{  
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode status = svcLocator->service("MessageSvc", m_msgSvc);
  MsgStream log(m_msgSvc, "DeadMaterial");
  
  FADS::MaterialManager* mmanager=FADS::MaterialManager::GetMaterialManager();

  if(status.isFailure())
    log << MSG::FATAL << "MessageSvc not found!" << endreq;
  else
    log << MSG::DEBUG << "MessageSvc initialized" << endreq;

  double dzdeadmaterial = (m_deadmaterial->zLength)/2.  *CLHEP::mm;
  double dydeadmaterial = (m_deadmaterial->yLength)/2.  *CLHEP::mm;
  double dxdeadmaterial = (m_deadmaterial->xLength)/2.  *CLHEP::mm;

  log << MSG::DEBUG << "DeadMaterial geometry " <<  dzdeadmaterial << " " << dydeadmaterial << " " << dxdeadmaterial << endreq;
 
  G4Box* wall = new G4Box("DeadMaterial",dxdeadmaterial,dydeadmaterial,dzdeadmaterial);
  G4LogicalVolume* wallV = new G4LogicalVolume(wall,mmanager->GetMaterial("Air"),"DeadMaterial");

  // ------- Create iron bar and place it inside the mother box --------
  double IronWidth = 5./2. *CLHEP::mm;
  for (int j=0; j<2; j++)
    {
      // X Iron plates
    G4Box* ironLayerX = new G4Box("IronLayerX",IronWidth,dydeadmaterial,dzdeadmaterial);
    G4LogicalVolume* ironLayerXV = new G4LogicalVolume(ironLayerX,mmanager->GetMaterial("Iron"),"DMIronLayerX");

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
    double IronWidthX = 140./2. *CLHEP::mm;
    G4Box* ironLayerZ = new G4Box("IronLayerZ",IronWidthX,dydeadmaterial,IronWidth);
    G4LogicalVolume* ironLayerZV = new G4LogicalVolume(ironLayerZ,mmanager->GetMaterial("Iron"),"DMIronLayerZ");
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
  envelope.theEnvelope=wallV;
}
