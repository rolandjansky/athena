/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4DetectorEnvelopes/Atlas.h"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "G4Material.hh"
#include "G4Tubs.hh"
#include "FadsMaterial/MaterialManager.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "FadsGeometry/DetectorFacilityBaseT.h"

#include "G4DetectorEnvelopes/EnvelopeGeometryManager.h"

static FADS::DetectorFacilityBaseT<Atlas> atl("AtlasDetector");   

void Atlas::BuildGeometry()
{

	EnvelopeGeometryManager *gm=EnvelopeGeometryManager::GetGeometryManager();

	G4double i_r=gm->AtlasInnerRadius() ;
	G4double o_r=gm->AtlasOuterRadius() ;
	G4double l =gm->AtlasMaxZ() ;
	std::cout<<" Atlas being built: "<<i_r<<" "<<o_r<<" "<<l<<std::endl;
	FADS::MaterialManager* mmanager=FADS::MaterialManager::GetMaterialManager();
	G4Material* mat=mmanager->GetMaterial("Air");
	G4Tubs* solid=new G4Tubs(name,i_r,o_r,l,0.,360.*CLHEP::deg);
	G4LogicalVolume *lv=new G4LogicalVolume(solid,mat,name);

// 	G4UserLimits *ul=new G4UserLimits;
// 	ul->SetMaxAllowedStep(5*cm);
//	lv->GetLogicalVolume()->SetUserLimits(ul);

	envelope.theEnvelope=lv;	

}

