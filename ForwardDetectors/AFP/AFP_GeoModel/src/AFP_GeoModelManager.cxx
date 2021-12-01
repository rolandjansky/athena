/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
  return volume.size(); 
}

PVConstLink AFP_GeoModelManager::getTreeTop(unsigned int i) const
{
  return volume[i];
}

void  AFP_GeoModelManager::addTreeTop(PVLink vol)
{
  volume.push_back(vol);
}
