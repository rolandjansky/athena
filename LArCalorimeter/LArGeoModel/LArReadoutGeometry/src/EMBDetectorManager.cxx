/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"


#include "LArReadoutGeometry/EMBDetDescr.h"
#include "LArReadoutGeometry/EMBDetectorRegion.h"
#include "LArReadoutGeometry/EMBBasicReadoutNumbers.h"

#include "LArReadoutGeometry/EMBDetectorManager.h"
#include "LArHV/LArHVManager.h"

class OrderBySampling {
    
public:
  
  inline bool operator () (const EMBDetectorRegion * a, const EMBDetectorRegion * b) const {
    return a->getSamplingIndex() < b->getSamplingIndex();
  }

};

EMBDetectorManager::EMBDetectorManager(const EMBHVManager& hvManager
				       , const EMBPresamplerHVManager& presamplerHVManager)
  : m_hvManager(hvManager)
  , m_presamplerHVManager(presamplerHVManager)
{
  setName("LArEMB");


  for (unsigned int r=0;r<2;r++) {
    for (unsigned int s=0;s<4;s++) {
      for (unsigned int e=0;e<2;e++) {
	m_DetRegionsRandom[r][s][e]=NULL;
      }
    } 
  }


  m_basicReadoutNumbers = new EMBBasicReadoutNumbers();
  m_accordionDetails    = NULL;
}


EMBDetectorManager::~EMBDetectorManager()
{
  for (unsigned int i=0;i<getNumDetectorRegions();i++) delete m_DetRegionsIterative[i];
  for (unsigned int i=0;i<getNumTreeTops();i++) getTreeTop(i)->unref();
  delete m_basicReadoutNumbers;
  delete m_accordionDetails;
}



EMBDetectorManager::DetectorRegionConstIterator EMBDetectorManager::beginDetectorRegion () const
{
  return m_DetRegionsIterative.begin();
}

EMBDetectorManager::DetectorRegionConstIterator EMBDetectorManager::endDetectorRegion () const
{
  return m_DetRegionsIterative.end();
}

const EMBDetectorRegion * EMBDetectorManager::getDetectorRegion (unsigned int endcap, unsigned int sampling, unsigned int region) const
{
  return m_DetRegionsRandom[region][sampling][endcap];
}

unsigned int EMBDetectorManager::getNumDetectorRegions () const
{
  return m_DetRegionsIterative.size();
}

PVConstLink EMBDetectorManager::getTreeTop (unsigned int i) const
{
  return m_treeTop[i];
}

unsigned int EMBDetectorManager::getNumTreeTops () const
{
  return m_treeTop.size();
}

void EMBDetectorManager::addDetectorRegion (const EMBDetectorRegion *region)
{
  m_DetRegionsIterative.push_back(region);
  m_DetRegionsRandom[region->getRegionIndex()][region->getSamplingIndex()][region->getEndcapIndex()] = region;


  
  OrderBySampling order;
  std::sort(m_DetRegionsIterative.begin(),m_DetRegionsIterative.end(), order);

}

void EMBDetectorManager::addTreeTop (PVLink treeTop)
{
  m_treeTop.push_back(treeTop);
  treeTop->ref();
}

const EMBHVManager& EMBDetectorManager::getHVManager () const
{
  return m_hvManager;
}


const EMBPresamplerHVManager&  EMBDetectorManager::getPresamplerHVManager () const
{
  return m_presamplerHVManager;
}
