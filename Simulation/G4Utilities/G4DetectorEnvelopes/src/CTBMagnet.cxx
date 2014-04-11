/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4DetectorEnvelopes/CTBMagnet.h"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "CLHEP/Units/PhysicalConstants.h"

#include "FadsGeometry/DetectorFacilityBaseT.h"

static FADS::DetectorFacilityBaseT<CTBMagnet> b("CTBMagnet");   

CTBMagnet::CTBMagnet(G4String s):FADS::DetectorFacility(s),theVolume(0)
{
	dx=100*CLHEP::cm;
	dy=100*CLHEP::cm;
	dz=100*CLHEP::cm;
	boreDx=10*CLHEP::cm;
	boreDy=10*CLHEP::cm;
	material="Iron";
}

void CTBMagnet::BuildGeometry()
{
	G4Box *magnet=new G4Box("CTBMagnet",dx,dy,dz);
	G4Box *bore=new G4Box("MagnetBore",boreDx,boreDy,dz);
	G4VSolid *ctbMagnet=new G4SubtractionSolid("CTBMagnet",magnet,bore);
	theVolume=new G4LogicalVolume(ctbMagnet,0,name);
	SetMaterial(material);
	SetDetectorEnvelope(theVolume);
}
void CTBMagnet::SetDx(double d)
{	
	dx=d;
	if (theVolume) 
	{
		G4Box *b=(G4Box *)(theVolume->GetSolid());
		b->SetXHalfLength(d);
	}
}
void CTBMagnet::SetDy(double d)
{
	dy=d;
	if (theVolume) 
	{
		G4Box *b=(G4Box *)(theVolume->GetSolid());
		b->SetYHalfLength(d);
	}
}
void CTBMagnet::SetDz(double d)
{
	dz=d;
	if (theVolume) 
	{
		G4Box *b=(G4Box *)(theVolume->GetSolid());
		b->SetZHalfLength(d);
	}
}
void CTBMagnet::SetBoreDx(double d)
{
	boreDx=d;
	if (theVolume) 
	{
		G4BooleanSolid *b=(G4BooleanSolid *)(theVolume->GetSolid());
		G4Box *bb=(G4Box *)b->GetConstituentSolid(1);
		bb->SetXHalfLength(d);
	}
}
void CTBMagnet::SetBoreDy(double d)
{
	boreDy=d;
	if (theVolume) 
	{
		G4BooleanSolid *b=(G4BooleanSolid *)(theVolume->GetSolid());
		G4Box *bb=(G4Box *)b->GetConstituentSolid(1);
		bb->SetYHalfLength(d);
	}
}
#include "FadsMaterial/MaterialManager.h"
void CTBMagnet::SetMaterial(std::string mat)
{
	material=mat;
	if (theVolume) 
	{
		FADS::MaterialManager *mm=FADS::MaterialManager::GetMaterialManager();
		G4Material *gm=mm->GetMaterial(mat);
		theVolume->SetMaterial(gm);
	}
}

