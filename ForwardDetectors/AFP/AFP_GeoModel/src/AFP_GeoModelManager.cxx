/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_GeoModel/AFP_GeoModelManager.h"
AFP_GeoModelManager::AFP_GeoModelManager()
{
	setName(std::string("AFP_GeoModel"));
}


AFP_GeoModelManager::~AFP_GeoModelManager()
{
}


unsigned int AFP_GeoModelManager::getNumTreeTops() const
{
  return m_volume.size(); 
}

PVConstLink AFP_GeoModelManager::getTreeTop(unsigned int i) const
{
  return m_volume[i];
}

void  AFP_GeoModelManager::addTreeTop(PVLink vol){
  m_volume.push_back(vol);
}
