/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_GeoModel/ALFA_DetectorManager.h"

ALFA_DetectorManager::ALFA_DetectorManager()
{
  setName("ALFA");
}


ALFA_DetectorManager::~ALFA_DetectorManager()
{
  for (unsigned int i = 0; i < m_volume.size(); i++)
    m_volume[i]->unref();
}


unsigned int ALFA_DetectorManager::getNumTreeTops() const
{
  return m_volume.size();
}

PVConstLink ALFA_DetectorManager::getTreeTop(unsigned int i) const
{
  return m_volume[i];
}

void  ALFA_DetectorManager::addTreeTop(PVLink vol)
{
  vol->ref();
  m_volume.push_back(vol);
}



