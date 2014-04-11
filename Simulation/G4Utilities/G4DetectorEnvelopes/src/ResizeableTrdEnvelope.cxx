/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4DetectorEnvelopes/ResizeableTrdEnvelope.h"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "G4Trd.hh"
#include "G4SubtractionSolid.hh"
#include "CLHEP/Units/PhysicalConstants.h"

#include "FadsGeometry/DetectorFacilityBaseT.h"

static FADS::DetectorFacilityBaseT<ResizeableTrdEnvelope> b("ResizeableTrd");   

ResizeableTrdEnvelope::ResizeableTrdEnvelope(G4String s):DetectorFacility(s),theVolume(0)
{
	dx1=100*CLHEP::cm;
	dx2=100*CLHEP::cm;
	dy1=100*CLHEP::cm;
	dy2=100*CLHEP::cm;
	dz=100*CLHEP::cm;
	material="Air";
}

void ResizeableTrdEnvelope::BuildGeometry()
{
	G4VSolid *solid= new G4Trd(name,dx1,dx2,dy1,dy2,dz);
	theVolume=new G4LogicalVolume(solid,0,name);
	SetMaterial(material);
	SetDetectorEnvelope(theVolume);
}
void ResizeableTrdEnvelope::SetDx1(double d)
{	
	dx1=d;
	if (theVolume) 
	{
		G4Trd *b=(G4Trd *)(theVolume->GetSolid());
		b->SetXHalfLength1(d);
	}
}

void ResizeableTrdEnvelope::SetDx2(double d)
{	
	dx2=d;
	if (theVolume) 
	{
		G4Trd *b=(G4Trd *)(theVolume->GetSolid());
		b->SetXHalfLength2(d);
	}
}

void ResizeableTrdEnvelope::SetDy1(double d)
{
	dy1=d;
	if (theVolume) 
	{
		G4Trd *b=(G4Trd *)(theVolume->GetSolid());
		b->SetYHalfLength1(d);
	}
}

void ResizeableTrdEnvelope::SetDy2(double d)
{
	dy2=d;
	if (theVolume) 
	{
		G4Trd *b=(G4Trd *)(theVolume->GetSolid());
		b->SetYHalfLength2(d);
	}
}

void ResizeableTrdEnvelope::SetDz(double d)
{
	dz=d;
	if (theVolume) 
	{
		G4Trd *b=(G4Trd *)(theVolume->GetSolid());
		b->SetZHalfLength(d);
	}
}

#include "FadsMaterial/MaterialManager.h"
void ResizeableTrdEnvelope::SetMaterial(std::string mat)
{
	material=mat;
	if (theVolume) 
	{
		FADS::MaterialManager *mm=FADS::MaterialManager::GetMaterialManager();
		G4Material *gm=mm->GetMaterial(mat);
		theVolume->SetMaterial(gm);
	}
}

