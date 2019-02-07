/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArReadoutGeometry/HECDetDescr.h"
#include "LArReadoutGeometry/HECDetectorRegion.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "LArReadoutGeometry/HECDetectorManager.h"
#include "LArHV/LArHVManager.h"
#include "StoreGate/StoreGate.h"

// Class HECDetectorManager 

HECDetectorManager::HECDetectorManager(bool isTestBeam)
  :GeoVDetectorManager(),m_HVManager(NULL),m_isTestBeam(isTestBeam)
{
  setName("LArHEC");

  ISvcLocator *svcLocator = Gaudi::svcLocator();
  IRDBAccessSvc* rdbAccess;
  IGeoModelSvc * geoModel;
  
  if (svcLocator->service ("GeoModelSvc",geoModel) == StatusCode::FAILURE)
    throw std::runtime_error("Error in HECDetectorManager, cannot access GeoModelSvc");

  if(svcLocator->service ("RDBAccessSvc",rdbAccess) == StatusCode::FAILURE)
    throw std::runtime_error("Error in HECDetectorManager, cannot access RDBAccessSvc");

  DecodeVersionKey larVersionKey(geoModel, "LAr");
  IRDBRecordset_ptr hecLongBlock          = rdbAccess->getRecordsetPtr("HecLongitudinalBlock", larVersionKey.tag(),larVersionKey.node());
  IRDBRecordset_ptr hecPad                = rdbAccess->getRecordsetPtr("HecPad", larVersionKey.tag(),larVersionKey.node());
  IRDBRecordset_ptr hadronicEndcap        = rdbAccess->getRecordsetPtr("HadronicEndcap", larVersionKey.tag(), larVersionKey.node());

  if(hecLongBlock->size()==0)
  {
    hecLongBlock          = rdbAccess->getRecordsetPtr("HecLongitudinalBlock", "HecLongitudinalBlock-00");
    if (hecLongBlock->size()==0)         throw std::runtime_error("Error getting HecLongitudinalBlock table");
  }

  if(hecPad->size()==0)
  {
    hecPad                = rdbAccess->getRecordsetPtr("HecPad","HecPad-00");
    if (hecPad->size()==0)               throw std::runtime_error("Error getting HecPad table");
  }

  if(hadronicEndcap->size()==0)
  {
    hadronicEndcap        = rdbAccess->getRecordsetPtr("HadronicEndcap","HadronicEndcap-00");
    if (hadronicEndcap->size()==0)       throw std::runtime_error("Error getting HadronicEndcap table");
  }

  if (hecPad->size()!=hecLongBlock->size()) throw std::runtime_error("Error.  Hec[LongitudinalBlock,Pad] size discrepancy");

  // Get the focal length:
  m_focalToRef1 = (*hadronicEndcap)[0]->getDouble("ZORIG")*Gaudi::Units::cm;
  m_focalToRef2 = m_focalToRef1;
  double betweenWheel=(*hadronicEndcap)[0]->getDouble("GAPWHL")*Gaudi::Units::cm;
  if(!m_isTestBeam) m_focalToRef2 += betweenWheel;

  for (unsigned int b=0;b<hecLongBlock->size();b++) {
    double etaBoundary[15];
    const IRDBRecord *block = (*hecLongBlock)[b];
    unsigned int blockNumber= (unsigned int) (block->getDouble("IBLC")+0.01); // will truncate down.
    double innerRadius= block->getDouble("BLRMN")*Gaudi::Units::cm;
    double outerRadius= block->getDouble("BLRMX")*Gaudi::Units::cm;
    double depth= block->getDouble("BLDPTH")*Gaudi::Units::cm;
    unsigned int numLArGaps= (unsigned int) (block->getDouble("BLMOD") + 0.01); // will truncate down.
    double frontPlateThickness= block->getDouble("PLATE0")*Gaudi::Units::cm;
    double backPlateThickness= block->getDouble("PLATEE")*Gaudi::Units::cm;
    
    const IRDBRecord *pad = (*hecPad)[b];
    for (int j=0;j<15;j++) etaBoundary[j]=pad->getDouble("ETA",j);
    HECLongBlock *longBlock = new HECLongBlock(blockNumber, 
					       innerRadius, 
					       outerRadius,
					       depth, 
					       numLArGaps, 
					       frontPlateThickness, 
					       backPlateThickness,
					       etaBoundary);
    m_HecLongBlock.push_back(longBlock);
    if ((!m_isTestBeam) && b<3) m_focalToRef2 += depth;
  }

  
}


HECDetectorManager::~HECDetectorManager()
{
  for (unsigned int i=0;i<getNumBlocks();i++) delete m_HecLongBlock[i];
  for (unsigned int i=0;i<getNumDetectorRegions();i++) delete m_DetRegionsIterative[i];
  for (unsigned int i=0;i<getNumTreeTops();i++) getTreeTop(i)->unref();
}



HECDetectorManager::DetectorRegionConstIterator HECDetectorManager::beginDetectorRegion () const
{
  return m_DetRegionsIterative.begin();
}

HECDetectorManager::DetectorRegionConstIterator HECDetectorManager::endDetectorRegion () const
{
  return m_DetRegionsIterative.end();
}

const HECDetectorRegion * HECDetectorManager::getDetectorRegion (unsigned int endcap, unsigned int sampling, unsigned int region) const
{
  return m_DetRegionsRandom[endcap][sampling][region];
}

unsigned int HECDetectorManager::getNumDetectorRegions () const
{
  return m_DetRegionsIterative.size();
}

PVConstLink HECDetectorManager::getTreeTop (unsigned int i) const
{
  return m_treeTop[i];
}

unsigned int HECDetectorManager::getNumTreeTops () const
{
  return m_treeTop.size();
}

HECDetectorManager::BlockConstIterator HECDetectorManager::beginBlock () const
{
  return m_HecLongBlock.begin();
}

HECDetectorManager::BlockConstIterator HECDetectorManager::endBlock () const
{
  return m_HecLongBlock.end();
}

const HECLongBlock * HECDetectorManager::getBlock (unsigned int i) const
{
  return m_HecLongBlock[i];
}

unsigned int HECDetectorManager::getNumBlocks () const
{
  return m_HecLongBlock.size();
}

void HECDetectorManager::addDetectorRegion (const HECDetectorRegion *region)
{
  m_DetRegionsRandom[region->getEndcapIndex()][region->getSamplingIndex()][region->getRegionIndex()] = region;
  m_DetRegionsIterative.push_back(region);
}

void HECDetectorManager::addTreeTop (PVLink treeTop)
{
  m_treeTop.push_back(treeTop);
  treeTop->ref();
}


const HECHVManager * HECDetectorManager::getHVManager () const
{

  if (!m_HVManager) {
    StoreGateSvc *detStore = StoreGate::pointer("DetectorStore");
    const LArHVManager *manager = NULL;
    if (detStore->retrieve(manager)==StatusCode::SUCCESS) {
      m_HVManager=manager->getHECHVManager();
    }
  }
  return m_HVManager;
}


