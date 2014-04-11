/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4DetectorEnvelopes/Muon.h"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"
#include "G4Polycone.hh"
#include "G4Material.hh"
#include "FadsGeometry/DetectorFacilityBaseT.h"
#include "FadsMaterial/MaterialManager.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "G4DetectorEnvelopes/EnvelopeGeometryManager.h"

static FADS::DetectorFacilityBaseT<Muon> muon("Muon"); 

void Muon::BuildGeometry()
{

	EnvelopeGeometryManager *gm=EnvelopeGeometryManager::GetGeometryManager();

	std::cout<<"Constructing the Muon System envelope"<<std::endl;
	std::cout<<"Inner Radius "<<gm->MuonInnerRadius()<<std::endl;
	std::cout<<"Outer Radius "<<gm->MuonOuterRadius()<<std::endl;
	std::cout<<"Length       "<<gm->MuonMaxZ()<<std::endl;

	
	G4double ir=gm->CaloOuterRadius();
	G4double pir=gm->MuonInnerRadius();
	G4double ors=gm->MuonOuterRadius();
	G4double l =gm->MuonMaxZ();
	G4double eff=gm->CaloMaxZ();
	
	G4double v1[]={-l,-eff,-eff,+eff,+eff,+l};
	G4double v2[]={pir,pir,ir,ir,pir,pir};
	G4double v3[]={ors,ors,ors,ors,ors,ors};
	
	G4Polycone *c4=new G4Polycone(name,0,360*CLHEP::deg,6,v1,v2,v3);
	FADS::MaterialManager *mmanager=FADS::MaterialManager::GetMaterialManager();
	G4LogicalVolume* lv=new G4LogicalVolume(c4,mmanager->GetMaterial("Air"),name);	

	envelope.theEnvelope=lv;


}

