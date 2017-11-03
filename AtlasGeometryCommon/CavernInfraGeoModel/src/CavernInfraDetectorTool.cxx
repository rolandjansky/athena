/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CavernInfraDetectorTool.h"
#include "CavernInfraDetectorFactory.h" 
#include "CavernInfraDetectorFactory01.h" 
#include "CavernInfraGeoModel/CavernInfraDetectorManager.h" 

#include "GeoModelInterfaces/IGeoDbTagSvc.h"
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
  

StatusCode CavernInfraDetectorTool::create()
{ 
  IGeoDbTagSvc *geoDbTag;
  StatusCode sc = service ("GeoDbTagSvc",geoDbTag);
  if(sc.isFailure()) {
    msg(MSG::ERROR) << "Could not locate GeoDbTagSvc" << endmsg;
    return sc;
  }

  std::string cavernInfraVersion = geoDbTag->cavernInfraVersion();
  msg(MSG::INFO) << "Building Cavern geometry version " << cavernInfraVersion << endmsg;
  if(cavernInfraVersion.empty()) {
    msg(MSG::INFO) << "No Cavern Infra version for the given configuration. Skip building CavernInfraGeoModel" << endmsg;
    return StatusCode::SUCCESS;
  }

  std::string versionNode = "CavernInfra";

  DataHandle<GeoModelExperiment> theExpt; 
  if (StatusCode::SUCCESS != detStore()->retrieve(theExpt,"ATLAS")) { 
    msg(MSG::ERROR) << "Could not find GeoModelExperiment ATLAS" << endmsg; 
    return StatusCode::FAILURE; 
  } 
 
  GeoPhysVol *world=&*theExpt->getPhysVol();
  IRDBAccessSvc* raccess = 0;
  sc = service("RDBAccessSvc",raccess);
  if(sc.isFailure()) {
    msg(MSG::ERROR) << "Could not locate RDBAccessSvc" << endmsg;
    return sc;
  }

  if(cavernInfraVersion.find("CavernInfra")==0) {
    std::string geoVersion = cavernInfraVersion.substr(12,2);
    if(geoVersion=="00" || geoVersion=="01") {
      msg(MSG::ERROR) << "ERROR. Version " << cavernInfraVersion << " is obsolete and cannot be supported anymore" << endmsg;
      return StatusCode::FAILURE;
    }
    else if(geoVersion=="02") {
      // This factory is chosen by CavernInfra-02 tag, but in fact it uses data from CavernInfra-01
      CavernInfraDetectorFactory01 theCavernInfraFactory01(detStore().operator->(),raccess);
      theCavernInfraFactory01.setTagNode(cavernInfraVersion,versionNode);
      theCavernInfraFactory01.create(world);
      m_manager = theCavernInfraFactory01.getDetectorManager();
    }
  }

  if(!m_manager) {
    // If geometry has not been built yet fall back to the default factory
    CavernInfraDetectorFactory theCavernInfraFactory(detStore().operator->(),raccess);
    theCavernInfraFactory.setTagNode(cavernInfraVersion,versionNode);
    theCavernInfraFactory.create(world);
    m_manager = theCavernInfraFactory.getDetectorManager();
  }

  if(m_manager) {
    theExpt->addManager(m_manager);
    sc = detStore()->record(m_manager,
			  m_manager->getName());
    if(sc.isFailure()) {
      msg(MSG::ERROR) << "Could not register CavernInfra detector manager" << endmsg;
      return sc;
    }
  }
  else {
    msg(MSG::ERROR) << "ERROR. Failed to build Cavern Version " << cavernInfraVersion << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode CavernInfraDetectorTool::clear()
{
  SG::DataProxy* _proxy = detStore()->proxy(ClassID_traits<CavernInfraDetectorManager>::ID(),m_manager->getName());
  if(_proxy) {
    _proxy->reset();
    m_manager = 0;
  }
  return StatusCode::SUCCESS;
}

