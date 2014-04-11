/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4DetectorEnvelopes/Calorimeter.h"
#include "FadsGeometry/DetectorFacilityBaseT.h"
#include "FadsMaterial/MaterialManager.h"
#include "G4Polycone.hh"
#include "CLHEP/Units/PhysicalConstants.h"

#include "G4DetectorEnvelopes/EnvelopeGeometryManager.h"

static FADS::DetectorFacilityBaseT<Calorimeter> calo("Calorimeter"); 

void Calorimeter::BuildGeometry()
{

	EnvelopeGeometryManager *gm=EnvelopeGeometryManager::GetGeometryManager();

	std::cout<<"Constructing the calorimeter envelope "<<std::endl;
	std::cout<<"Inner Radius "<<gm->CaloInnerRadius()<<std::endl;
	std::cout<<"Outer Radius "<<gm->CaloOuterRadius()<<std::endl;
	std::cout<<"Length       "<<gm->CaloMaxZ()<<std::endl;
	
	G4double ir=gm->IdetOuterRadius();
	G4double pir=gm->CaloInnerRadius();
	G4double ors=gm->CaloOuterRadius();
	G4double l =gm->CaloMaxZ();
	G4double eff=gm->IdetMaxZ();
	
	G4double v1[]={-l,-eff,-eff,+eff,+eff,+l};
	G4double v2[]={pir,pir,ir,ir,pir,pir};
	G4double v3[]={ors,ors,ors,ors,ors,ors};
	
	G4Polycone *c4=new G4Polycone(name,0,360*CLHEP::deg,6,v1,v2,v3);
	FADS::MaterialManager* mmanager=FADS::MaterialManager::GetMaterialManager();
	G4LogicalVolume* lv=new G4LogicalVolume(c4,mmanager->GetMaterial("Air"),name);	

	envelope.theEnvelope=lv;

	  
}

