/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4DetectorEnvelopes/ResizeableBoxEnvelope.h"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "CLHEP/Units/PhysicalConstants.h"

#include "FadsGeometry/DetectorFacilityBaseT.h"

static FADS::DetectorFacilityBaseT<ResizeableBoxEnvelope> b("ResizeableBox");   

ResizeableBoxEnvelope::ResizeableBoxEnvelope(G4String s):FADS::DetectorFacility(s),theVolume(0)
{
	dx=100*CLHEP::cm;
	dy=100*CLHEP::cm;
	dz=100*CLHEP::cm;
	material="Air";
}

void ResizeableBoxEnvelope::BuildGeometry()
{
	G4VSolid *solid= new G4Box(name,dx,dy,dz);
	if (holes.size())
	{
		HoleSize::const_iterator it;
		for (it=holes.begin();it!=holes.end();it++)
		{
			G4ThreeVector size=(*it).second;
			std::string n=(*it).first;
			G4VSolid *temp=new G4Box("",size.x(),size.y(),size.z());
			G4ThreeVector tPos;
			if (holesPos.find(n) != holesPos.end())
			{
				tPos=holesPos[n];
			}
			solid=new G4SubtractionSolid(name,solid,temp,0,tPos);
		}
	}
	theVolume=new G4LogicalVolume(solid,0,name);
	SetMaterial(material);
	SetDetectorEnvelope(theVolume);
}
void ResizeableBoxEnvelope::SetDx(double d)
{	
	dx=d;
	if (theVolume) 
	{
		G4Box *b=(G4Box *)(theVolume->GetSolid());
		b->SetXHalfLength(d);
	}
}
void ResizeableBoxEnvelope::SetDy(double d)
{
	dy=d;
	if (theVolume) 
	{
		G4Box *b=(G4Box *)(theVolume->GetSolid());
		b->SetYHalfLength(d);
	}
}
void ResizeableBoxEnvelope::SetDz(double d)
{
	dz=d;
	if (theVolume) 
	{
		G4Box *b=(G4Box *)(theVolume->GetSolid());
		b->SetZHalfLength(d);
	}
}

void ResizeableBoxEnvelope::DefineSubtractionBox(std::string name)
{
	G4ThreeVector temp;
	holes[name]=temp;
}
void ResizeableBoxEnvelope::SetSubtractionBoxDimensions(std::string name, G4ThreeVector& pos)
{
	if (holes.find(name)==holes.end()) DefineSubtractionBox(name);
	holes[name]=pos;
}
void ResizeableBoxEnvelope::SetSubtractionBoxPosition(std::string name, G4ThreeVector& pos)
{
	if (holes.find(name)==holes.end())
	{
		std::cout<<" Warning = box "<<name<<" not found!"<<std::endl;
		return;
	}
	holesPos[name]=pos;
}
#include "FadsMaterial/MaterialManager.h"
void ResizeableBoxEnvelope::SetMaterial(std::string mat)
{
	material=mat;
	if (theVolume) 
	{
		FADS::MaterialManager *mm=FADS::MaterialManager::GetMaterialManager();
		G4Material *gm=mm->GetMaterial(mat);
		theVolume->SetMaterial(gm);
	}
}

