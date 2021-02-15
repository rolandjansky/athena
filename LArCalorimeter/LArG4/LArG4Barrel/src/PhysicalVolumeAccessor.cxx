/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PhysicalVolumeAccessor.h"
#include "G4LogicalVolumeStore.hh"
#include <string>
#include <assert.h>

G4LogicalVolume* PhysicalVolumeAccessor::GetLV(const std::string& name)
{
  G4LogicalVolumeStore *lvs=G4LogicalVolumeStore::GetInstance();
  for (unsigned int i=0;i<lvs->size();i++)
    {
      std::string lname=((lvs->operator[](i))->GetName());
      if (name==lname)
        return (lvs->operator[](i));
    }
  return nullptr;
}

PhysicalVolumeAccessor::PhysicalVolumeAccessor(const std::string& name)
{
  m_theLogicalVolume=GetLV(name);
}

PhysicalVolumeAccessor::PhysicalVolumeAccessor(const std::string& name,
                                               const std::string& PVname)
{
  m_theLogicalVolume=GetLV(name);
  assert (m_theLogicalVolume!=nullptr);
  for (unsigned int i=0;i<m_theLogicalVolume->GetNoDaughters();i++)
    {
      G4VPhysicalVolume *pv=m_theLogicalVolume->GetDaughter(i);
      if (PVname == static_cast<const std::string&>(pv->GetName()))
        {
          m_thePhysicalVolumes[pv->GetCopyNo()]=pv;
        }
    }
}

const G4VPhysicalVolume* PhysicalVolumeAccessor::GetPhysicalVolume(int icopy) const
{
  auto physVolIter = m_thePhysicalVolumes.find(icopy);
  if (physVolIter!=m_thePhysicalVolumes.end())
    return physVolIter->second;
  else
    {
      return nullptr;
    }
}

void PhysicalVolumeAccessor::SetPhysicalVolumeList(const std::string& name)
{
  // assert (m_thePhysicalVolumes.size()==0);
  for (unsigned int i=0;i<m_theLogicalVolume->GetNoDaughters();i++)
    {
      G4VPhysicalVolume *pv=m_theLogicalVolume->GetDaughter(i);
      if (name == static_cast<const std::string&>(pv->GetName()))
        {
          m_thePhysicalVolumes[pv->GetCopyNo()]=pv;
        }
    }
}
