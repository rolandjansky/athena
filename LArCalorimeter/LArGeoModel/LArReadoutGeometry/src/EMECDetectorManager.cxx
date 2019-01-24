/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "LArReadoutGeometry/EMECDetectorManager.h"
#include "LArReadoutGeometry/EMECDetectorRegion.h"
#include "LArReadoutGeometry/EMECDetDescr.h"
#include "StoreGate/StoreGate.h"
#include "LArHV/LArHVManager.h"
// Class EMECDetectorManager 

EMECDetectorManager::EMECDetectorManager()
  :m_MagicNumbers(new EMECMagicNumbers())
{
  setName("LArEMEC");
  
  for (unsigned int r=0;r<6;r++) {
    for (unsigned int s=0;s<4;s++) {
      for (unsigned int io=0;io<2;io++) {
	for (unsigned int e=0;e<2;e++) {
	  m_DetRegionsRandom[r][s][io][e]=NULL;
	}
      }
    }
  }

  m_HVManager[0]=NULL;
  m_HVManager[1]=NULL;
  m_presamplerHVManager=NULL;

  // The EMEC gets and managers certain arrays needed to build descriptors.  Here is that:
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  IRDBAccessSvc* rdbAccess;
  IGeoModelSvc * geoModel;
  
  if (svcLocator->service ("GeoModelSvc",geoModel) == StatusCode::FAILURE)
    throw std::runtime_error("Error in LArDetectorFactoryH62002, cannot access GeoModelSvc");

  if(svcLocator->service ("RDBAccessSvc",rdbAccess) == StatusCode::FAILURE)
    throw std::runtime_error("Error in LArDetectorFactoryH62002, cannot access RDBAccessSvc");

  DecodeVersionKey larVersionKey(geoModel, "LAr");
  IRDBRecordset_ptr emecSamplingSep       = rdbAccess->getRecordsetPtr("EmecSamplingSep", larVersionKey.tag(),larVersionKey.node());
  if (emecSamplingSep->size()==0)   throw std::runtime_error("Error getting EmecSamplingSep table");

  const IRDBRecord *ess = (*emecSamplingSep)[0];
  for (int j=0;j<7;j++)  m_ziw.push_back(ess->getDouble("ZIW",j)*Gaudi::Units::cm);
  for (int j=0;j<44;j++) m_zsep12.push_back(ess->getDouble("ZSEP12",j)*Gaudi::Units::cm);
  for (int j=0;j<22;j++) m_zsep23.push_back(ess->getDouble("ZSEP23",j)*Gaudi::Units::cm);

  IRDBRecordset_ptr emecMagicNumbers       = rdbAccess->getRecordsetPtr("EmecMagicNumbers", larVersionKey.tag(),larVersionKey.node());
  if (emecMagicNumbers->size()==0) {
    emecMagicNumbers       = rdbAccess->getRecordsetPtr("EmecMagicNumbers", "EmecMagicNumbers-00");
    if (emecMagicNumbers->size()==0) throw std::runtime_error("Error getting EmecMagicNumbers table");
  }
  m_MagicNumbers->focalToRef        =(*emecMagicNumbers)[0]->getDouble("FOCALTOREF")*Gaudi::Units::mm;
  m_MagicNumbers->refToActive       =(*emecMagicNumbers)[0]->getDouble("REFTOACTIVE")*Gaudi::Units::mm;
  m_MagicNumbers->activeLength      =(*emecMagicNumbers)[0]->getDouble("ACTIVELENGTH")*Gaudi::Units::mm;
  m_MagicNumbers->refToPresampler   =(*emecMagicNumbers)[0]->getDouble("REFTOPRESAMPLER")*Gaudi::Units::mm;
  m_MagicNumbers->presamplerLength  =(*emecMagicNumbers)[0]->getDouble("PRESAMPLERLENGTH")*Gaudi::Units::mm;
  
}


EMECDetectorManager::~EMECDetectorManager()
{
  for (unsigned int i=0;i<getNumDetectorRegions();i++) delete m_DetRegionsIterative[i];
  for (unsigned int i=0;i<getNumTreeTops();i++) getTreeTop(i)->unref();
  delete m_MagicNumbers;
}



//## Other Operations (implementation)
EMECDetectorManager::DetectorRegionConstIterator EMECDetectorManager::beginDetectorRegion () const
{
  return m_DetRegionsIterative.begin();
}

EMECDetectorManager::DetectorRegionConstIterator EMECDetectorManager::endDetectorRegion () const
{
  return m_DetRegionsIterative.end();
}

const EMECDetectorRegion * EMECDetectorManager::getDetectorRegion (unsigned int endcap, unsigned int sampling, unsigned int region, bool inner) const
{
  return m_DetRegionsRandom[region][sampling][inner ? 1 : 0][endcap];
}

unsigned int EMECDetectorManager::getNumDetectorRegions () const
{
  return m_DetRegionsIterative.size();
}

PVConstLink EMECDetectorManager::getTreeTop (unsigned int i) const
{
 return m_treeTop[i];
}

unsigned int EMECDetectorManager::getNumTreeTops () const
{
  return m_treeTop.size();
}

void EMECDetectorManager::addDetectorRegion (const EMECDetectorRegion *region)
{
  m_DetRegionsIterative.push_back(region);
  m_DetRegionsRandom[region->getRegionIndex()][region->getSamplingIndex()][region->getRadialIndex()][region->getEndcapIndex()] = region;
}

void EMECDetectorManager::addTreeTop (PVLink treeTop)
{
  m_treeTop.push_back(treeTop);
  treeTop->ref();
}

const EMECHVManager * EMECDetectorManager::getHVManager (EMECHVManager::IOType io) const
{

  if (!m_HVManager[io]) {
    StoreGateSvc *detStore = StoreGate::pointer("DetectorStore");
    const LArHVManager *manager = NULL;
    if (detStore->retrieve(manager)==StatusCode::SUCCESS) {
      m_HVManager[io]=manager->getEMECHVManager(io);
    }
  } 
  return m_HVManager[io];
}

const EMECPresamplerHVManager * EMECDetectorManager::getPresamplerHVManager () const
{

  if (!m_presamplerHVManager) {
    StoreGateSvc *detStore = StoreGate::pointer("DetectorStore");
    const LArHVManager *manager = NULL;
    if (detStore->retrieve(manager)==StatusCode::SUCCESS) {
      m_presamplerHVManager=manager->getEMECPresamplerHVManager();
    }
  } 
  return m_presamplerHVManager;
}
