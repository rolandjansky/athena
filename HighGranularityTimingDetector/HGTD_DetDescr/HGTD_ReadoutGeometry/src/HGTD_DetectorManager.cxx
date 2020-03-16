/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"

HGTD_DetectorManager::HGTD_DetectorManager(){}

HGTD_DetectorManager::~HGTD_DetectorManager(){}

unsigned int HGTD_DetectorManager::getNumTreeTops() const
{
    return m_volume.size();
}

PVConstLink HGTD_DetectorManager::getTreeTop(unsigned int i) const
{
    return m_volume[i];
}

void HGTD_DetectorManager::addTreeTop(PVLink vol){
    vol->ref();
    m_volume.push_back(vol);
}
