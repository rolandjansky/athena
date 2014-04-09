/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4DetectorEnvelopes/MuonSystemEntryLayer.h"
#include "G4Polycone.hh"
#include "FadsGeometry/DetectorFacilityBaseT.h"
#include "FadsMaterial/MaterialManager.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "G4DetectorEnvelopes/EnvelopeGeometryManager.h"

static FADS::DetectorFacilityBaseT<MuonSystemEntryLayer> muon("MuonSystemEntryLayer"); 

void MuonSystemEntryLayer::BuildGeometry()
{

	EnvelopeGeometryManager *gm=EnvelopeGeometryManager::GetGeometryManager();

	G4double ir=gm->CaloOuterRadius();
	G4double pir=gm->MuonInnerRadius();
	G4double ors=ir+1*CLHEP::mm;
//	G4double l =gm->MuonMaxZ(); // Don't actually need this to define entry layer
	G4double eff=gm->CaloMaxZ();
	
	G4double v1[]={-eff-1*CLHEP::mm,-eff,-eff,+eff,+eff,+eff+1*CLHEP::mm};
	G4double v2[]={pir,pir,ir,ir,pir,pir};
	G4double v3[]={ors,ors,ors,ors,ors,ors};
	
	G4Polycone *c4=new G4Polycone(name,0,360*CLHEP::deg,6,v1,v2,v3);
	FADS::MaterialManager* mmanager=FADS::MaterialManager::GetMaterialManager();
	G4LogicalVolume* lv=new G4LogicalVolume(c4,mmanager->GetMaterial("Air"),name);	

	envelope.theEnvelope=lv;

}

