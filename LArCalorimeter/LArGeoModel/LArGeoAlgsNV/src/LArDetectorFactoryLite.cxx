/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArDetectorFactoryLite.h"
#include "LArReadoutGeometryBuilder.h"
#include "LArGeoBarrel/ElStraightSectionBuilder.h"
#include "LArGeoFcal/FCALChannelMapBuilder.h"
#include "LArHV/LArHVManager.h"
#include "LArGeoEndcap/MbtsReadoutBuilder.h"
#include "LArGeoCode/VDetectorParameters.h"
#include "LArGeoRAL/RAL.h"

#include "LArReadoutGeometry/HECDetectorManager.h"
#include "LArReadoutGeometry/FCALDetectorManager.h"
#include "LArReadoutGeometry/EMECDetectorManager.h"
#include "LArReadoutGeometry/EMBDetectorManager.h"

#include "GeoModelRead/ReadGeoModel.h"
#include "GeoModelKernel/GeoPVLink.h"
#include "GeoModelKernel/GeoVolumeCursor.h"
#include "GeoModelUtilities/StoredAlignX.h"
#include "GeoModelUtilities/StoredPhysVol.h"

#include "AthenaKernel/getMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#define SYSTEM_OF_UNITS Gaudi::Units

LArGeo::LArDetectorFactoryLite::LArDetectorFactoryLite(StoreGateSvc* detStore
						       , IRDBAccessSvc* paramSvc
						       , GeoModelIO::ReadGeoModel* sqliteReader
						       , const LArHVManager* hvManager)
  : AthMessaging(Athena::getMessageSvc(), "LArDetectorFactoryLite")
  , m_detectorManager(nullptr)
  , m_detStore(detStore)
  , m_paramSvc(paramSvc)
  , m_sqliteReader(sqliteReader)
  , m_hvManager(hvManager)
  , m_barrelSagging(false)
  , m_testBeam(0)
{
}

LArGeo::LArDetectorFactoryLite::~LArDetectorFactoryLite()
{
}


void LArGeo::LArDetectorFactoryLite::create(GeoPhysVol* world)
{
  ATH_MSG_INFO("LArDetectorFactoryLite::create()");

  std::string errorMessage{""};

  if(LArGeo::buildFcalChannelMap(m_detStore,m_paramSvc,Athena::getMessageSvc()).isFailure()) {
    errorMessage="Failed to build FCAL Channel Map";
    ATH_MSG_FATAL(errorMessage);
    throw std::runtime_error(errorMessage);
  }

  // Build Electrode straight sections in the barrel
  if(LArGeo::buildElStraightSections(m_detStore
				     , m_paramSvc
				     , Athena::getMessageSvc()
				     , m_barrelSagging) != StatusCode::SUCCESS) {
    errorMessage="Failed to build LAr Barrel electrode sections";
    ATH_MSG_FATAL(errorMessage);
    throw std::runtime_error(errorMessage);
  }

  // Get the list of alignable transforms from SQLite, and record them into DetStore
  std::map<std::string, GeoAlignableTransform*> mapAXF = m_sqliteReader->getPublishedNodes<std::string, GeoAlignableTransform*>("LAr");
  for( auto& [key,xf] : mapAXF) {
    StoredAlignX *sAlignX = new StoredAlignX(xf);
    if(m_detStore->record(sAlignX,key)!=StatusCode::SUCCESS) {
      errorMessage="Failed to record StoredAlignX for the key: "+key;
      ATH_MSG_FATAL(errorMessage);
      throw std::runtime_error(errorMessage);
    }
  }

  // Get the list of full phys volumes from SQLite, and record them into DetStore
  std::map<std::string, GeoFullPhysVol*> mapFPV = m_sqliteReader->getPublishedNodes<std::string, GeoFullPhysVol*>("LAr");
  for( auto& [key,pv] : mapFPV) {
    StoredPhysVol *sPhysVol = new StoredPhysVol(pv);
    if(m_detStore->record(sPhysVol,key)!=StatusCode::SUCCESS) {
      errorMessage="Failed to record StoredPhysVol for the key: " + key;
      ATH_MSG_FATAL(errorMessage);
      throw std::runtime_error(errorMessage);
    }
  }

  // Build LAr readout geometry
  double projectivityDisplacement{0.};
  IRDBRecordset_ptr emecGeometry =  m_paramSvc->getRecordsetPtr("EmecGeometry","");
  projectivityDisplacement = (*emecGeometry)[0]->getDouble("ZSHIFT");

  auto subDetManagers = buildLArReadoutGeometry(m_detStore
						, m_hvManager
						, Athena::getMessageSvc()
						, m_testBeam
						, projectivityDisplacement);

  EMBDetectorManager*  embDetectorManager{std::get<0>(subDetManagers)};
  EMECDetectorManager* emecDetectorManager{std::get<1>(subDetManagers)};
  HECDetectorManager*  hecDetectorManager{std::get<2>(subDetManagers)};
  FCALDetectorManager* fcalDetectorManager{std::get<3>(subDetManagers)};

  if(!embDetectorManager
     || !emecDetectorManager
     || !hecDetectorManager
     || !fcalDetectorManager) {
    errorMessage="Failed to build LAr Readout Geometry description";
    ATH_MSG_FATAL(errorMessage);
    throw std::runtime_error(errorMessage);
  }

  m_detectorManager = new LArDetectorManager(embDetectorManager,emecDetectorManager,hecDetectorManager,fcalDetectorManager);
  m_detectorManager->isTestBeam(false);

  // Build MBTS readout geometry
  IRDBRecordset_ptr mbtsTrds  = m_paramSvc->getRecordsetPtr("MBTSTrds","");
  IRDBRecordset_ptr mbtsTubs  = m_paramSvc->getRecordsetPtr("MBTSTubs","");
  IRDBRecordset_ptr mbtsPcons = m_paramSvc->getRecordsetPtr("MBTSPcons","");
  IRDBRecordset_ptr cryoPcons = m_paramSvc->getRecordsetPtr("CryoPcons","");


  IRDBRecordset::const_iterator first,last; 
  double zposMM = 0.;

  if(mbtsPcons->size()==0) {
    first = mbtsTubs->begin();
    last = mbtsTubs->end();
    for(; first!=last; first++) {
      if((*first)->getString("TUBE") == "MBTS_mother") {
	zposMM = (*first)->getDouble("ZPOS")*SYSTEM_OF_UNITS::mm;
	break;
      }
    }
  }
  else {
    double zStartCryoMother = 0.;
    first = cryoPcons->begin();
    last = cryoPcons->end();
    for(; first!=last; ++first) {
      if((*first)->getString("PCON")=="Endcap::CryoMother" 
	 && (*first)->getInt("PLANE_ID")==0) {
	zStartCryoMother = (*first)->getDouble("ZPLANE");
	break;
      }
    }

    double zStartMM = 0.;
    first = mbtsPcons->begin();
    last = mbtsPcons->end();
    for(; first!=last; ++first) {
      if((*first)->getString("PCON")=="MBTS::Mother"
	 && (*first)->getInt("PLANE_ID")==0) {
	zStartMM = (*first)->getDouble("ZPLANE");
	break;
      }
    }
    
    zposMM = zStartCryoMother - zStartMM;
  }


  std::map<std::string,unsigned> trdMap;
  for(unsigned indTrd(0);indTrd<mbtsTrds->size();++indTrd) {
    std::string keyTrd = (*mbtsTrds)[indTrd]->getString("TRD");
    trdMap[keyTrd]=indTrd;
  }

  if(LArGeo::buildMbtsReadout(m_detStore
                              , m_paramSvc
                              , Athena::getMessageSvc()
                              , zposMM
                              , trdMap
                              , std::string()
                              , std::string()).isFailure()) {
    errorMessage="Failed to build MBTS Readout Geometry description";
    ATH_MSG_FATAL(errorMessage);
    throw std::runtime_error(errorMessage);
  }

  // Set Tree Tops
  GeoVolumeCursor cursor(world);
  while(!cursor.atEnd()) {
    std::string volName = cursor.getName();
    if(volName.compare(0,3,"LAr")==0) {
      m_detectorManager->addTreeTop(GeoPVLink(cursor.getVolume().operator->()));
    }
    cursor.next();
  }

}

