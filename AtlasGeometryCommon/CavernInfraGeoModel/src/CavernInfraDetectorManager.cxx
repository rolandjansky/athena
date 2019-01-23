/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "CavernInfraGeoModel/CavernInfraDetectorManager.h"

CavernInfraDetectorManager::CavernInfraDetectorManager()
{
  setName("CavernInfra");
}


CavernInfraDetectorManager::~CavernInfraDetectorManager()
{
  for(unsigned int i=0; i<m_treeTops.size(); i++)
    m_treeTops[i]->unref();
}


unsigned int CavernInfraDetectorManager::getNumTreeTops() const
{
  return m_treeTops.size();
}

PVConstLink CavernInfraDetectorManager::getTreeTop(unsigned int i) const
{
  if(i<m_treeTops.size())
    return m_treeTops[i];
  else
    return 0;
}

void  CavernInfraDetectorManager::addTreeTop(PVLink link) 
{
  link->ref();
  m_treeTops.push_back(link);
}

