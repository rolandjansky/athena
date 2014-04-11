/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4DetectorEnvelopes/InnerDetector.h"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"
#include "G4Tubs.hh"
#include "FadsGeometry/DetectorFacilityBaseT.h"
#include "FadsMaterial/MaterialManager.h"
#include "CLHEP/Units/PhysicalConstants.h"


#include "G4DetectorEnvelopes/EnvelopeGeometryManager.h"

static FADS::DetectorFacilityBaseT<InnerDetector> id("InnerDetector"); 

void InnerDetector::BuildGeometry()
{

	EnvelopeGeometryManager *gm=EnvelopeGeometryManager::GetGeometryManager();
//	std::cout<<"IDET mother volume. IRAD "<<gm->IdetInnerRadius()<<std::endl;
	G4double irad=gm->IdetInnerRadius();
	G4double orad=gm->IdetOuterRadius();
	G4double l =gm->IdetMaxZ();
	FADS::MaterialManager *mmanager=FADS::MaterialManager::GetMaterialManager();
	G4Tubs *solid=new G4Tubs(name,irad,orad,l,0,360.*CLHEP::deg);
	G4LogicalVolume* lv=new G4LogicalVolume(solid,mmanager->GetMaterial("Air"),name);
	
	envelope.theEnvelope=lv;
}

