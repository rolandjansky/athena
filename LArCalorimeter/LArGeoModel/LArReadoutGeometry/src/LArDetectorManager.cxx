/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArReadoutGeometry/LArDetectorManager.h"
#include <algorithm>

LArDetectorManager::LArDetectorManager()
  : m_embManager (0),
    m_emecManager (0),
    m_hecManager (0),
    m_fcalManager (0),
    m_isTestBeam(false)
{
}

LArDetectorManager::LArDetectorManager (const EMBDetectorManager* emb
					, const EMECDetectorManager* emec
					, const HECDetectorManager* hec
					, const FCALDetectorManager* fcal)
  : m_embManager(emb)
  , m_emecManager(emec)
  , m_hecManager(hec)
  , m_fcalManager(fcal)
  , m_isTestBeam(true)
{
  setName("LArMgr");
}

LArDetectorManager::~LArDetectorManager()
{
  unsigned int ntree = LArDetectorManager::getNumTreeTops();
  for (unsigned int i=0;i<ntree;i++) LArDetectorManager::getTreeTop(i)->unref();
}

PVConstLink LArDetectorManager::getTreeTop (unsigned int i) const
{
  return m_treeTop[i];
}

unsigned int LArDetectorManager::getNumTreeTops () const
{
  return m_treeTop.size();
}

void LArDetectorManager::addTreeTop (PVLink treeTop)
{
  if (std::find(m_treeTop.begin(),m_treeTop.end(),treeTop)!=m_treeTop.end())  return;
  m_treeTop.push_back(treeTop);
  treeTop->ref();
}

