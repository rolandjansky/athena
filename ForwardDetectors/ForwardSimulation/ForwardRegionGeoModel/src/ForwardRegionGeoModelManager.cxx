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
  return m_volume.size(); 
}

PVConstLink ForwardRegionGeoModelManager::getTreeTop(unsigned int i) const
{
  return m_volume[i];
}

void  ForwardRegionGeoModelManager::addTreeTop(PVLink vol){
  m_volume.push_back(vol);
}
