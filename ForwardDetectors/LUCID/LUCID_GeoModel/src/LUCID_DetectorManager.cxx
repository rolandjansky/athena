/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_GeoModel/LUCID_DetectorManager.h"

LUCID_DetectorManager::LUCID_DetectorManager() {

  setName("LUCID");
}

LUCID_DetectorManager::~LUCID_DetectorManager() {

  for (unsigned int i=0; i<m_volume.size(); i++) m_volume[i]->unref();
}

unsigned int LUCID_DetectorManager::getNumTreeTops() const {

  return m_volume.size();
}

PVConstLink LUCID_DetectorManager::getTreeTop(unsigned int i) const {

  return m_volume[i];
}

void  LUCID_DetectorManager::addTreeTop(PVLink vol) {

  vol->ref();
  m_volume.push_back(vol);
}
