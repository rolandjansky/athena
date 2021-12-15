/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ServiceHandle.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "LArReadoutGeometry/EMECDetectorManager.h"
#include "LArReadoutGeometry/EMECDetectorRegion.h"
#include "LArReadoutGeometry/EMECDetDescr.h"
#include "StoreGate/StoreGateSvc.h"
#include "LArHV/LArHVManager.h"
// Class EMECDetectorManager 

EMECDetectorManager::EMECDetectorManager(const EMECHVManager* hvManagerInner
					 , const EMECHVManager* hvManagerOuter
					 , const EMECPresamplerHVManager* presamplerHVManager)
  : m_MagicNumbers(new EMECMagicNumbers())
  , m_presamplerHVManager(presamplerHVManager)
{
  setName("LArEMEC");
  
  for (unsigned int r=0;r<6;r++) {
    for (unsigned int s=0;s<4;s++) {
      for (unsigned int io=0;io<2;io++) {
	for (unsigned int e=0;e<2;e++) {
	  m_DetRegionsRandom[r][s][io][e]=nullptr;
	}
      }
    }
  }

  m_HVManager[0].store (hvManagerInner);
  m_HVManager[1].store (hvManagerOuter);

  // The EMEC gets and managers certain arrays needed to build descriptors.  Here is that:
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  IRDBAccessSvc *rdbAccess{nullptr};
  IGeoModelSvc  *geoModel{nullptr};
  IGeoDbTagSvc  *geoDbTagSvc{nullptr};

  if(svcLocator->service("GeoModelSvc",geoModel) == StatusCode::FAILURE)
    throw std::runtime_error("Error in HECDetectorManager, cannot access GeoModelSvc");

  if(svcLocator->service("GeoDbTagSvc",geoDbTagSvc) == StatusCode::FAILURE)
    throw std::runtime_error("Error in HECDetectorManager, cannot access GeoDbTagSvc");

  if(svcLocator->service(geoDbTagSvc->getParamSvcName(),rdbAccess) == StatusCode::FAILURE)
    throw std::runtime_error("Error in HECDetectorManager, cannot access RDBAccessSvc");

  std::string larKey, larNode;
  if(geoDbTagSvc->getSqliteReader()==nullptr) {
    DecodeVersionKey larVersionKey(geoModel, "LAr");
    larKey  = larVersionKey.tag();
    larNode = larVersionKey.node();
  }

  IRDBRecordset_ptr emecSamplingSep       = rdbAccess->getRecordsetPtr("EmecSamplingSep", larKey, larNode);
  if (emecSamplingSep->size()==0)   throw std::runtime_error("Error getting EmecSamplingSep table");

  const IRDBRecord *ess = (*emecSamplingSep)[0];
  for (int j=0;j<7;j++)  m_ziw.push_back(ess->getDouble("ZIW",j)*Gaudi::Units::cm);
  for (int j=0;j<44;j++) m_zsep12.push_back(ess->getDouble("ZSEP12",j)*Gaudi::Units::cm);
  for (int j=0;j<22;j++) m_zsep23.push_back(ess->getDouble("ZSEP23",j)*Gaudi::Units::cm);

  IRDBRecordset_ptr emecMagicNumbers       = rdbAccess->getRecordsetPtr("EmecMagicNumbers", larKey, larNode);
  if (emecMagicNumbers->size()==0) {
    emecMagicNumbers       = rdbAccess->getRecordsetPtr("EmecMagicNumbers", "EmecMagicNumbers-00");
    if (emecMagicNumbers->size()==0) {
      throw std::runtime_error("Error getting EmecMagicNumbers table");
    }
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
  unsigned int ntree = EMECDetectorManager::getNumTreeTops();
  for (unsigned int i=0;i<ntree;i++) EMECDetectorManager::getTreeTop(i)->unref();
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

const EMECHVManager& EMECDetectorManager::getHVManager (EMECHVManager::IOType io) const
{
  if(!m_HVManager[io].get()) {
    //Support lazy initialization for testbeams
    ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "HECHVManager");
    const LArHVManager *manager{nullptr};
    if (detStore->retrieve(manager)==StatusCode::SUCCESS) {
      m_HVManager[io].set (&(manager->getEMECHVManager(io)));
    }
  }
  return *(m_HVManager[io].get());
}

const EMECPresamplerHVManager& EMECDetectorManager::getPresamplerHVManager () const
{
  if (!m_presamplerHVManager.get()) {
    ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "HECHVManager");
    const LArHVManager *manager{nullptr};
    if (detStore->retrieve(manager)==StatusCode::SUCCESS) {
      m_presamplerHVManager.set (&(manager->getEMECPresamplerHVManager()));
    }
  } 
  return *m_presamplerHVManager.get();
}
