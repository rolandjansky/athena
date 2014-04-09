/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4DetectorEnvelopes/GenericPCONEnvelope.h"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "G4Polycone.hh"
#include "CLHEP/Units/PhysicalConstants.h"

#include "FadsGeometry/DetectorFacilityBaseT.h"

static FADS::DetectorFacilityBaseT<GenericPCONEnvelope> b("GenericPCON");   

GenericPCONEnvelope::GenericPCONEnvelope(G4String s):DetectorFacility(s),
						     theVolume(0)
{
	nsurfaces=0;
	phimin=0;
	deltaphi=360*CLHEP::deg;
	z_surface=0;
	rin_surface=0;
	rou_surface=0;
	material="Air";
}

void GenericPCONEnvelope::BuildGeometry()
{
	assert (nsurfaces);
	assert (z_surface && rin_surface && rou_surface);
	assert (sizeof(z_surface) == sizeof(rin_surface) && sizeof(rin_surface)
			== sizeof(rou_surface));
	G4Polycone *PCON=new G4Polycone(name,phimin,deltaphi,nsurfaces,
				        z_surface,rin_surface,rou_surface);
	theVolume=new G4LogicalVolume(PCON,0,name);
	SetMaterial(material);
	SetDetectorEnvelope(theVolume);
}
#include "FadsMaterial/MaterialManager.h"
void GenericPCONEnvelope::SetMaterial(std::string mat)
{
	material=mat;
	if (theVolume) 
	{
		FADS::MaterialManager *mm=FADS::MaterialManager::GetMaterialManager();
		G4Material *gm=mm->GetMaterial(mat);
		theVolume->SetMaterial(gm);
	}
}
