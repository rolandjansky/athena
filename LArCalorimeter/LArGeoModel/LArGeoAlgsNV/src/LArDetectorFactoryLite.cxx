/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArDetectorFactoryLite.h"
#include "LArGeoBarrel/ElStraightSectionBuilder.h"
#include "LArHV/LArHVManager.h"

#include "GeoModelRead/ReadGeoModel.h"
#include "GeoModelUtilities/StoredAlignX.h"
#include "GeoModelUtilities/StoredPhysVol.h"

#include "AthenaKernel/getMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

LArGeo::LArDetectorFactoryLite::LArDetectorFactoryLite(StoreGateSvc* detStore
						       , IRDBAccessSvc* paramSvc
						       , GeoModelIO::ReadGeoModel* sqliteReader
						       , const LArHVManager* /*hvManager*/)
  : AthMessaging(Athena::getMessageSvc(), "LArDetectorFactoryLite")
  , m_detectorManager(nullptr)
  , m_detStore(detStore)
  , m_paramSvc(paramSvc)
  , m_sqliteReader(sqliteReader)
  , m_barrelSagging(false)
{
}

LArGeo::LArDetectorFactoryLite::~LArDetectorFactoryLite()
{
}


void LArGeo::LArDetectorFactoryLite::create(GeoPhysVol* /*world*/)
{
  // Build Electrode straight sections in the barrel
  if(LArGeo::buildElStraightSections(m_detStore
				     , m_paramSvc
				     , Athena::getMessageSvc()
				     , m_barrelSagging) != StatusCode::SUCCESS) {
    ATH_MSG_FATAL("Failed to build LAr Barrel electrode sections");
    throw std::runtime_error("Failed to build LAr Barrel electrode sections");
  }

  // Get the list of alignable transforms from SQLite, and record them into DetStore
  std::map<std::string, GeoAlignableTransform*> mapAXF = m_sqliteReader->getPublishedNodes<std::string, GeoAlignableTransform*>("LAr");
  for( auto& [key,xf] : mapAXF) {
    StoredAlignX *sAlignX = new StoredAlignX(xf);
    if(m_detStore->record(sAlignX,key)!=StatusCode::SUCCESS) {
      ATH_MSG_FATAL("Failed to record StoredAlignX for the key: " << key);
      throw std::runtime_error(("Failed to record StoredAlignX for the key: " +key).c_str());
    }
  }

  // Get the list of full phys volumes from SQLite, and record them into DetStore
  std::map<std::string, GeoFullPhysVol*> mapFPV = m_sqliteReader->getPublishedNodes<std::string, GeoFullPhysVol*>("LAr");
  for( auto& [key,pv] : mapFPV) {
    StoredPhysVol *sPhysVol = new StoredPhysVol(pv);
    if(m_detStore->record(sPhysVol,key)!=StatusCode::SUCCESS) {
      ATH_MSG_FATAL("Failed to record StoredPhysVol for the key: " << key);
      throw std::runtime_error(("Failed to record StoredPhysVol for the key: " +key).c_str());
    }
  }

}
