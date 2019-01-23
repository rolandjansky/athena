/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "BeamPipeGeoModel/BeamPipeDetectorManager.h"

BeamPipeDetectorManager::BeamPipeDetectorManager()
{
  setName("BeamPipe");
}


BeamPipeDetectorManager::~BeamPipeDetectorManager()
{
  for (unsigned int i = 0; i < m_volume.size(); i++) {
    m_volume[i]->unref();
  }
}


unsigned int BeamPipeDetectorManager::getNumTreeTops() const
{
  return m_volume.size();
}

PVConstLink BeamPipeDetectorManager::getTreeTop(unsigned int i) const
{
  return m_volume[i];
}

void  BeamPipeDetectorManager::addTreeTop(PVLink vol)
{
  vol->ref();
  m_volume.push_back(vol);
}



