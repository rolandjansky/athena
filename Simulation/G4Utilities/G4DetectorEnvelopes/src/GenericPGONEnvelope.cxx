/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4DetectorEnvelopes/GenericPGONEnvelope.h"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "G4Polyhedra.hh"

#include "FadsGeometry/DetectorFacilityBaseT.h"
#include "CLHEP/Units/PhysicalConstants.h"

static FADS::DetectorFacilityBaseT<GenericPGONEnvelope> b("GenericPGON");   

GenericPGONEnvelope::GenericPGONEnvelope(G4String s):FADS::DetectorFacility(s),
						     theVolume(0)
{
	nsurfaces=0;
	nsides=6;
	phimin=0;
	deltaphi=360*CLHEP::deg;
	z_surface=0;
	rin_surface=0;
	rou_surface=0;
	material="Air";
}

void GenericPGONEnvelope::BuildGeometry()
{
	assert (nsurfaces);
	assert (z_surface && rin_surface && rou_surface);
	assert (sizeof(z_surface) == sizeof(rin_surface) && sizeof(rin_surface)
			== sizeof(rou_surface));
	G4Polyhedra *pgon=new G4Polyhedra(name,phimin,deltaphi,nsides,nsurfaces,
				        z_surface,rin_surface,rou_surface);
	theVolume=new G4LogicalVolume(pgon,0,name);
	SetMaterial(material);
	SetDetectorEnvelope(theVolume);
}
#include "FadsMaterial/MaterialManager.h"
void GenericPGONEnvelope::SetMaterial(std::string mat)
{
	material=mat;
	if (theVolume) 
	{
		FADS::MaterialManager *mm=FADS::MaterialManager::GetMaterialManager();
		G4Material *gm=mm->GetMaterial(mat);
		theVolume->SetMaterial(gm);
	}
}
