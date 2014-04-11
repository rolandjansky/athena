/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4DetectorEnvelopes/ResizeableTubsEnvelope.h"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "G4Tubs.hh"
#include "CLHEP/Units/PhysicalConstants.h"

#include "FadsGeometry/DetectorFacilityBaseT.h"

static FADS::DetectorFacilityBaseT<ResizeableTubsEnvelope> b("ResizeableTubs");   

ResizeableTubsEnvelope::ResizeableTubsEnvelope(G4String s):FADS::DetectorFacility(s),theVolume(0)
{
	rmin=0*CLHEP::cm;
	rmax=100*CLHEP::cm;
	dz=100*CLHEP::cm;
	startphi=0;
	deltaphi=360*CLHEP::deg;
	material="Air";
}

void ResizeableTubsEnvelope::BuildGeometry()
{
	G4Tubs* solid = new G4Tubs(name,rmin,rmax,dz,startphi,deltaphi);
	theVolume=new G4LogicalVolume(solid,0,name);
	SetMaterial(material);
	SetDetectorEnvelope(theVolume);
}
void ResizeableTubsEnvelope::SetRmin(double d)
{	
	rmin=d;
	if (theVolume) 
	{
		G4Tubs *b=(G4Tubs *)(theVolume->GetSolid());
		b->SetInnerRadius(d);
	}
}
void ResizeableTubsEnvelope::SetRmax(double d)
{
	rmax=d;
	if (theVolume) 
	{
		G4Tubs *b=(G4Tubs *)(theVolume->GetSolid());
		b->SetOuterRadius(d);
	}
}
void ResizeableTubsEnvelope::SetDz(double d)
{
	dz=d;
	if (theVolume) 
	{
		G4Tubs *b=(G4Tubs *)(theVolume->GetSolid());
		b->SetZHalfLength(d);
	}
}
void ResizeableTubsEnvelope::SetStartPhi(double d)
{
	startphi=d;
	if (theVolume) 
	{
		G4Tubs *b=(G4Tubs *)(theVolume->GetSolid());
		b->SetStartPhiAngle(d);
	}
}
void ResizeableTubsEnvelope::SetDeltaPhi(double d)
{
	deltaphi=d;
	if (theVolume) 
	{
		G4Tubs *b=(G4Tubs *)(theVolume->GetSolid());
		b->SetDeltaPhiAngle(d);
	}
}

#include "FadsMaterial/MaterialManager.h"
void ResizeableTubsEnvelope::SetMaterial(std::string mat)
{
	material=mat;
	if (theVolume) 
	{
		FADS::MaterialManager *mm=FADS::MaterialManager::GetMaterialManager();
		G4Material *gm=mm->GetMaterial(mat);
		theVolume->SetMaterial(gm);
	}
}
