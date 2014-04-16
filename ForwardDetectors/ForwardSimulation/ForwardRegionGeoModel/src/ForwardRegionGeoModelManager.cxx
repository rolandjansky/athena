/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardRegionGeoModel/ForwardRegionGeoModelManager.h"
ForwardRegionGeoModelManager::ForwardRegionGeoModelManager()
{
  setName("ForwardRegionGeoModel");
}


ForwardRegionGeoModelManager::~ForwardRegionGeoModelManager()
{
}


unsigned int ForwardRegionGeoModelManager::getNumTreeTops() const
{
  return volume.size(); 
}

PVConstLink ForwardRegionGeoModelManager::getTreeTop(unsigned int i) const
{
  return volume[i];
}

void  ForwardRegionGeoModelManager::addTreeTop(PVLink vol){
  volume.push_back(vol);
}
