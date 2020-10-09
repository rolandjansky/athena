/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"
#include "LArReadoutGeometry/FCALDetectorManager.h"
#include "LArReadoutGeometry/FCAL_ChannelMap.h"
#include "LArHV/LArHVManager.h"

FCALDetectorManager::FCALDetectorManager (const FCALHVManager* hvManager)
  : GeoVDetectorManager()
  , m_HVManager(hvManager)
{
  setName("LArFCAL");
  for (int s=0;s<2;s++) {
    for (int m=0;m<3;m++) {
      m_Module[s][m]=NULL;
    }
  }

  ISvcLocator *svcLocator = Gaudi::svcLocator();

  StoreGateSvc *detStore;
  if (svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in FCALDetectorManager: cannot access DetectorStore");
  }

  const FCAL_ChannelMap *cMap;
  if (detStore->retrieve(cMap)==StatusCode::FAILURE) {
    throw std::runtime_error("You must construct the GeoModel FCAL before instantiating FCALDetectorManager");
  }

  m_fcal_channel_map=cMap;

}


FCALDetectorManager::~FCALDetectorManager()
{
  for (int s=0;s<2;s++) {
    for (int m=0;m<3;m++) {
      delete m_Module[s][m];
    }
  }
  for (unsigned int i=0;i<getNumTreeTops();i++) getTreeTop(i)->unref();
}



FCALDetectorManager::ConstIterator FCALDetectorManager::beginFCAL () const
{
  return m_DetList.begin();
}

FCALDetectorManager::ConstIterator FCALDetectorManager::endFCAL () const
{
  return m_DetList.end();
}

const FCALModule* FCALDetectorManager::getFCAL (FCALModule::Module module, FCALModule::Endcap endcap) const
{
  return m_Module[endcap][module-1];
}

unsigned int FCALDetectorManager::getNumTreeTops () const
{
  return m_treeTop.size();
}

PVConstLink FCALDetectorManager::getTreeTop (unsigned int i) const
{
  return m_treeTop[i];
}

void FCALDetectorManager::addModule (FCALModule* fcalModule)
{
  int mod  = fcalModule->getModuleIndex()-1;
  int side = fcalModule->getEndcapIndex();
  m_Module[side][mod]=fcalModule;
  m_DetList.push_back(fcalModule);
  fcalModule->setManager(this);
}

void FCALDetectorManager::addTreeTop (PVLink treeTop)
{
  m_treeTop.push_back(treeTop);
  treeTop->ref();
}

const FCALHVManager& FCALDetectorManager::getHVManager () const
{
  return *m_HVManager;
}


