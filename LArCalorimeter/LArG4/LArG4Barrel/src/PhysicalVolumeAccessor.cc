/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4Barrel/PhysicalVolumeAccessor.h"
#include "G4LogicalVolumeStore.hh"
#include <string>
#include <assert.h>

G4LogicalVolume* PhysicalVolumeAccessor::GetLV(std::string name)
{
	G4LogicalVolumeStore *lvs=G4LogicalVolumeStore::GetInstance();
	for (unsigned int i=0;i<lvs->size();i++)
	{
		std::string lname=((lvs->operator[](i))->GetName());
		if (name==lname)
			return (lvs->operator[](i));
	}
//	std::cout<<"PhysicalVolumeAccessor::GetLV Warning!!! Volume "<<name
//               <<" not found!!! returning 0"<<std::endl;
	return 0;
}

PhysicalVolumeAccessor::PhysicalVolumeAccessor(std::string name)
{
	theLogicalVolume=GetLV(name);
}

PhysicalVolumeAccessor::PhysicalVolumeAccessor(std::string name,
						std::string PVname)
{
	theLogicalVolume=GetLV(name);
	assert (theLogicalVolume!=0);
	for (int i=0;i<theLogicalVolume->GetNoDaughters();i++)
	{
		G4VPhysicalVolume *pv=theLogicalVolume->GetDaughter(i);
		if (PVname==(pv->GetName()))
		{
			thePhysicalVolumes[pv->GetCopyNo()]=pv;
		}
	}
}

const G4VPhysicalVolume* PhysicalVolumeAccessor::GetPhysicalVolume(int icopy)
{
	if (thePhysicalVolumes.find(icopy)!=thePhysicalVolumes.end())
		return thePhysicalVolumes[icopy];
	else
	{
//              std::cout<<"Physical Volume copy "<<icopy<<" not found in"
//                       <<theLogicalVolume->GetName()<<"!!! return 0"<<std::endl;
		return 0;
	}
}

void PhysicalVolumeAccessor::SetPhysicalVolumeList(std::string name)
{
  //	assert (thePhysicalVolumes.size()==0);
	for (int i=0;i<theLogicalVolume->GetNoDaughters();i++)
	{
		G4VPhysicalVolume *pv=theLogicalVolume->GetDaughter(i);
		if (name==(pv->GetName()))
		{
			thePhysicalVolumes[pv->GetCopyNo()]=pv;
		}
	}
}
