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
	m_theLogicalVolume=GetLV(name);
}

PhysicalVolumeAccessor::PhysicalVolumeAccessor(std::string name,
						std::string PVname)
{
	m_theLogicalVolume=GetLV(name);
	assert (m_theLogicalVolume!=0);
	for (int i=0;i<m_theLogicalVolume->GetNoDaughters();i++)
	{
		G4VPhysicalVolume *pv=m_theLogicalVolume->GetDaughter(i);
		if (PVname==(pv->GetName()))
		{
			m_thePhysicalVolumes[pv->GetCopyNo()]=pv;
		}
	}
}

const G4VPhysicalVolume* PhysicalVolumeAccessor::GetPhysicalVolume(int icopy)
{
	if (m_thePhysicalVolumes.find(icopy)!=m_thePhysicalVolumes.end())
		return m_thePhysicalVolumes[icopy];
	else
	{
//              std::cout<<"Physical Volume copy "<<icopy<<" not found in"
//                       <<m_theLogicalVolume->GetName()<<"!!! return 0"<<std::endl;
		return 0;
	}
}

void PhysicalVolumeAccessor::SetPhysicalVolumeList(std::string name)
{
  //	assert (m_thePhysicalVolumes.size()==0);
	for (int i=0;i<m_theLogicalVolume->GetNoDaughters();i++)
	{
		G4VPhysicalVolume *pv=m_theLogicalVolume->GetDaughter(i);
		if (name==(pv->GetName()))
		{
			m_thePhysicalVolumes[pv->GetCopyNo()]=pv;
		}
	}
}
