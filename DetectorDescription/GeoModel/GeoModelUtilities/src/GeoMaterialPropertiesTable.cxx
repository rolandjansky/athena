/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelUtilities/GeoMaterialPropertiesTable.h"
#include "GeoModelUtilities/GeoMaterialPropertyVector.h"
#include <stdexcept>
#include <iostream>

GeoMaterialPropertiesTable::GeoMaterialPropertiesTable()
{
}

GeoMaterialPropertiesTable::~GeoMaterialPropertiesTable()
{
  MPTiterator i;
  for(i = m_MPT.begin(); i!= m_MPT.end(); i++) 
    delete (*i).second;

  m_MPT.clear();
  m_MPTC.clear();
}


void GeoMaterialPropertiesTable::AddConstProperty(const char     *key,
						  double PropertyValue)
{
  m_MPTC [std::string(key)] = PropertyValue;
}

void GeoMaterialPropertiesTable::AddProperty(const char     *key,
					     double* PhotonMomenta,
					     double* PropertyValues,
					     int     NumEntries)
{
  GeoMaterialPropertyVector *mpv = 
    new GeoMaterialPropertyVector(PhotonMomenta, 
				  PropertyValues, 
				  NumEntries);
  m_MPT[std::string(key)] = mpv;
}

void GeoMaterialPropertiesTable::AddProperty(const char *key,
					     GeoMaterialPropertyVector *mpv)
{
  m_MPT [std::string(key)] = mpv;
} 

void GeoMaterialPropertiesTable::RemoveConstProperty(const char *key)
{
  m_MPTC.erase(std::string(key));
}

void GeoMaterialPropertiesTable::RemoveProperty(const char *key)
{
  m_MPT.erase(std::string(key));
}

void GeoMaterialPropertiesTable::AddEntry(const char     *key,
					  double  aPhotonMomentum,
					  double  aPropertyValue)
{
  GeoMaterialPropertyVector *targetVector=m_MPT[std::string(key)];
  if(targetVector != 0) 
    targetVector->AddElement(aPhotonMomentum, aPropertyValue);
  else 
    throw std::runtime_error("GeoMaterialPropertiesTable::AddEntry ==> Material Property Vector not found.");
}

GeoMaterialPropertiesTable::GeoMatPVMap_ConstIt GeoMaterialPropertiesTable::beginPVMap() const
{
  return m_MPT.begin();
}

GeoMaterialPropertiesTable::GeoMatPVMap_ConstIt GeoMaterialPropertiesTable::endPVMap() const
{
  return m_MPT.end();
}

GeoMaterialPropertiesTable::GeoMatPMap_ConstIt GeoMaterialPropertiesTable::beginPMap() const
{
  return m_MPTC.begin();
}

GeoMaterialPropertiesTable::GeoMatPMap_ConstIt GeoMaterialPropertiesTable::endPMap() const
{
  return m_MPTC.end();
}

void GeoMaterialPropertiesTable::DumpTable() const
{
  for(GeoMatPVMap_ConstIt i=m_MPT.begin(); i!=m_MPT.end(); ++i) 
  {
    std::cout << (*i).first << "\n";
    if((*i).second != 0)
      (*i).second->DumpVector();
    else 
      std::cout << "NULL Material Property Vector Pointer." << "\n";
  }

  for (GeoMatPMap_ConstIt j = m_MPTC.begin(); j != m_MPTC.end(); ++j) 
  {
    std::cout << j->first << "\n";
    if(j->second != 0) 
      std::cout << j->second << "\n";
    else
      std::cout << "No Material Constant Property." << "\n";
  }
}

