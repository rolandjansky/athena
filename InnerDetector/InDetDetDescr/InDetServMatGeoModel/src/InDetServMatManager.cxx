/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/InDetServMatManager.h"

namespace InDetDD {

InDetServMatManager::InDetServMatManager( ) 
{
  setName("InDetServMat");
}





InDetServMatManager::~InDetServMatManager()
{
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
InDetServMatManager::addTreeTop(PVConstLink vol){
  m_volume.push_back(vol);
}


} // namespace InDetDD


