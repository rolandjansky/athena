/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/InDetServMatManager.h"

namespace InDetDD {

InDetServMatManager::InDetServMatManager( ) 
{
  setName("InDetServMat");
}





InDetServMatManager::~InDetServMatManager()
{
  for(unsigned int i=0; i<m_volume.size(); i++)
    m_volume[i]->unref();
}



unsigned int 
InDetServMatManager::getNumTreeTops() const
{
  return m_volume.size(); 
}

PVConstLink 
InDetServMatManager::getTreeTop(unsigned int i) const
{
  return m_volume[i];
}

void 
InDetServMatManager::addTreeTop(PVLink vol){
  m_volume.push_back(vol);
  vol->ref();
}


} // namespace InDetDD


