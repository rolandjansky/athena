/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CavernInfraGeoModel/CavernInfraDetectorTool.h"
#include "CavernInfraGeoModel/CavernInfraDetectorFactory.h" 
#include "CavernInfraGeoModel/CavernInfraDetectorFactory01.h" 
#include "CavernInfraGeoModel/CavernInfraDetectorManager.h" 

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

CavernInfraDetectorTool::CavernInfraDetectorTool( const std::string& type, 
						  const std::string& name,
						  const IInterface* parent )
  : GeoModelTool( type, name, parent ),
    m_manager(0)
{
}

CavernInfraDetectorTool::~CavernInfraDetectorTool()
{
}
  

StatusCode CavernInfraDetectorTool::create( StoreGateSvc* detStore )
{ 
  IGeoModelSvc *geoModel;
  StatusCode sc = service ("GeoModelSvc",geoModel);
  if(sc.isFailure()) {
    msg(MSG::ERROR) << "Could not locate GeoModelSvc" << endreq;
    return sc;
  }

  std::string cavernInfraVersion = geoModel->cavernInfraVersion();
  msg(MSG::INFO) << "Building Cavern geometry version " << cavernInfraVersion << endreq;
  if(cavernInfraVersion.empty()) {
    msg(MSG::INFO) << "No Cavern Infra version for the given configuration. Skip building CavernInfraGeoModel" << endreq;
    return StatusCode::SUCCESS;
  }

  std::string versionNode = "CavernInfra";

  DataHandle<GeoModelExperiment> theExpt; 
  if (StatusCode::SUCCESS != detStore->retrieve(theExpt,"ATLAS")) { 
    msg(MSG::ERROR) << "Could not find GeoModelExperiment ATLAS" << endreq; 
    return StatusCode::FAILURE; 
  } 
 
  GeoPhysVol *world=&*theExpt->getPhysVol();
  IRDBAccessSvc* raccess = 0;
  sc = service("RDBAccessSvc",raccess);
  if(sc.isFailure()) {
    msg(MSG::ERROR) << "Could not locate RDBAccessSvc" << endreq;
    return sc;
  }

  if(cavernInfraVersion.find("CavernInfra")==0) {
    std::string geoVersion = cavernInfraVersion.substr(12,2);
    if(geoVersion=="00" || geoVersion=="01") {
      msg(MSG::ERROR) << "ERROR. Version " << cavernInfraVersion << " is obsolete and cannot be supported anymore" << endreq;
      return StatusCode::FAILURE;
    }
    else if(geoVersion=="02") {
      // This factory is chosen by CavernInfra-02 tag, but in fact it uses data from CavernInfra-01
      CavernInfraDetectorFactory01 theCavernInfraFactory01(detStore,raccess);
      theCavernInfraFactory01.setTagNode(cavernInfraVersion,versionNode);
      theCavernInfraFactory01.create(world);
      m_manager = theCavernInfraFactory01.getDetectorManager();
    }
  }

  if(!m_manager) {
    // If geometry has not been built yet fall back to the default factory
    CavernInfraDetectorFactory theCavernInfraFactory(detStore,raccess);
    theCavernInfraFactory.setTagNode(cavernInfraVersion,versionNode);
    theCavernInfraFactory.create(world);
    m_manager = theCavernInfraFactory.getDetectorManager();
  }

  if(m_manager) {
    theExpt->addManager(m_manager);
    sc = detStore->record(m_manager,
			  m_manager->getName());
    if(sc.isFailure()) {
      msg(MSG::ERROR) << "Could not register CavernInfra detector manager" << endreq;
      return sc;
    }
  }
  else {
    msg(MSG::ERROR) << "ERROR. Failed to build Cavern Version " << cavernInfraVersion << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode CavernInfraDetectorTool::clear(StoreGateSvc* detStore)
{
  SG::DataProxy* _proxy = detStore->proxy(ClassID_traits<CavernInfraDetectorManager>::ID(),m_manager->getName());
  if(_proxy) {
    _proxy->reset();
    m_manager = 0;
  }
  return StatusCode::SUCCESS;
}

