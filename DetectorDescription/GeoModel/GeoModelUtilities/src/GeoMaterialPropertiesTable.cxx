/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  for(i = MPT.begin(); i!= MPT.end(); i++) 
    delete (*i).second;

  MPT.clear();
  MPTC.clear();
}


void GeoMaterialPropertiesTable::AddConstProperty(const char     *key,
						  double PropertyValue)
{
  MPTC [std::string(key)] = PropertyValue;
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
  MPT[std::string(key)] = mpv;
}

void GeoMaterialPropertiesTable::AddProperty(const char *key,
					     GeoMaterialPropertyVector *mpv)
{
  MPT [std::string(key)] = mpv;
} 

void GeoMaterialPropertiesTable::RemoveConstProperty(const char *key)
{
  MPTC.erase(std::string(key));
}

void GeoMaterialPropertiesTable::RemoveProperty(const char *key)
{
  MPT.erase(std::string(key));
}

void GeoMaterialPropertiesTable::AddEntry(const char     *key,
					  double  aPhotonMomentum,
					  double  aPropertyValue)
{
  GeoMaterialPropertyVector *targetVector=MPT[std::string(key)];
  if(targetVector != 0) 
    targetVector->AddElement(aPhotonMomentum, aPropertyValue);
  else 
    throw std::runtime_error("GeoMaterialPropertiesTable::AddEntry ==> Material Property Vector not found.");
}

GeoMaterialPropertiesTable::GeoMatPVMap_ConstIt GeoMaterialPropertiesTable::beginPVMap() const
{
  return MPT.begin();
}

GeoMaterialPropertiesTable::GeoMatPVMap_ConstIt GeoMaterialPropertiesTable::endPVMap() const
{
  return MPT.end();
}

GeoMaterialPropertiesTable::GeoMatPMap_ConstIt GeoMaterialPropertiesTable::beginPMap() const
{
  return MPTC.begin();
}

GeoMaterialPropertiesTable::GeoMatPMap_ConstIt GeoMaterialPropertiesTable::endPMap() const
{
  return MPTC.end();
}

void GeoMaterialPropertiesTable::DumpTable() const
{
  for(GeoMatPVMap_ConstIt i=MPT.begin(); i!=MPT.end(); i++) 
  {
    std::cout << (*i).first << "\n";
    if((*i).second != 0)
      (*i).second->DumpVector();
    else 
      std::cout << "NULL Material Property Vector Pointer." << "\n";
  }

  for (GeoMatPMap_ConstIt j = MPTC.begin(); j != MPTC.end(); ++j) 
  {
    std::cout << j->first << "\n";
    if(j->second != 0) 
      std::cout << j->second << "\n";
    else
      std::cout << "No Material Constant Property." << "\n";
  }
}

