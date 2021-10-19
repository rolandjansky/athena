/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TileGeoModel/TileDetectorTool.h"
#include "TileDetectorFactory.h"
#include "TileAtlasFactory.h"
#include "TileTBFactory.h"
#include "TileDetDescr/TileDetDescrManager.h" 
#include "TileDetDescr/TileDddbManager.h"
#include "TileConditions/TileCablingService.h"

#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "AthenaKernel/ClassID_traits.h"
#include "SGTools/DataProxy.h"

TileDetectorTool::TileDetectorTool(const std::string& type, 
				   const std::string& name, 
				   const IInterface* parent):
  GeoModelTool(type, name, parent),
  m_switches(false,true),
  m_not_locked(true),
  m_useNewFactory(true),
  m_geometryConfig("FULL"),
  m_manager(0)
{
  declareProperty("UseNewFactory", m_useNewFactory);
  declareProperty("GeometryConfig",m_geometryConfig);
  declareProperty("TestBeam", m_switches.testBeam);
  // declareProperty("AddPlatesToCellVolume", m_switches.addPlatesToCell); to make user aware that the value is taken from DB rather from JO
  declareProperty("Ushape",m_switches.uShape);
  declareProperty("Glue",m_switches.glue);
  declareProperty("PVT",m_switches.pvt);
  declareProperty("Steel",m_switches.steel);
  declareProperty("CsTube",m_switches.csTube);
}

TileDetectorTool::~TileDetectorTool()
{
}

void TileDetectorTool::setSwitch(int & param, int value, const char * name)
{
    if (param < 0) {
      param = value;
      ATH_MSG_INFO(name << " parameter from database is: " << param);
    } else {
      if (param != value) {
        ATH_MSG_WARNING("Overriding " << name << " value from DB by value from jobOptions, using "
                        << param << " instead of " << value);
      } else {
        ATH_MSG_INFO(name << " parameter from jobOptions is: " << param);
      }
    }
}


StatusCode TileDetectorTool::create()
{ 
  MsgStream log(msgSvc(), name());
  ATH_MSG_INFO(" Entering TileDetectorTool::create()");

  // Get the detector configuration
  IGeoModelSvc *geoModel;
  ATH_CHECK(service ("GeoModelSvc",geoModel));

  std::string atlasVersion = geoModel->atlasVersion();
  std::string tileVersion = geoModel->tileVersionOverride();
 
  std::string versionTag = tileVersion.empty()? atlasVersion : tileVersion;
  std::string versionNode = tileVersion.empty()? "ATLAS" : "TileCal";
 
  if(versionTag == "AUTO")
  {
    versionTag = "TileCal-00";
    versionNode = "TileCal";	
  }
  if (atlasVersion.compare(0,9,"ATLAS-CTB") == 0 || tileVersion.compare(0,6,"TileTB") == 0) {
    ATH_MSG_INFO("CTB geometry detected: " << atlasVersion  << " " << tileVersion);
    m_switches.testBeam = true;
  }
  
  //Locate the top level experiment node
  GeoModelExperiment* theExpt = nullptr;
  CHECK( detStore()->retrieve(theExpt, "ATLAS") );

  if ( 0 == m_detector )
  {
    IRDBAccessSvc* raccess = 0;
    ATH_CHECK(service("RDBAccessSvc",raccess));

    TileDddbManager_ptr  dbManager(new TileDddbManager(raccess,versionTag,versionNode));
    m_manager = new TileDetDescrManager(dbManager);

    if (0==dbManager->GetNumberOfEnv() && m_useNewFactory) {
      ATH_MSG_WARNING("New TileAtlasFactory can not be used because TileGlobals do not exist in Database");
      ATH_MSG_WARNING("Use old TileDetectorFactory instead");
      m_useNewFactory = false;
    }

    CHECK( initIds() );

    m_switches.addPlatesToCell = dbManager->addPlatesToCell();
    setSwitch(m_switches.uShape, dbManager->uShape(), "Ushape");
    setSwitch(m_switches.glue,   dbManager->glue(),   "Glue");
    setSwitch(m_switches.pvt,    dbManager->PVT(),    "PVT");
    setSwitch(m_switches.steel,  dbManager->steel(),  "Steel");
    setSwitch(m_switches.csTube, dbManager->csTube(), "CsTube");

    m_not_locked = false;
    
    GeoPhysVol *world=&*theExpt->getPhysVol();
    if(m_switches.testBeam)
    {
      TileTBFactory theTileTBFactory(detStore().operator->(),m_manager,m_switches,&log);
      theTileTBFactory.create(world);
    }
    else if (m_useNewFactory)
    {
      TileAtlasFactory theTileFactory(detStore().operator->(),m_manager,m_switches,&log,m_geometryConfig=="FULL");
      theTileFactory.create(world);
    }
    else
    {
      TileDetectorFactory theTileFactory(detStore().operator->(),m_manager,m_switches,&log);
      theTileFactory.create(world);
    }

    CHECK( createElements() );

    // Register the TileDetDescrManager instance with the Transient Detector Store
    CHECK( detStore()->record(m_manager, m_manager->getName()) );
    theExpt->addManager(m_manager);

    // For reco jobs: release DB manager. Cannot do it here for simulation jobs, they
    // release DB manager as part of global GeoModel release
    if(m_geometryConfig=="RECO")
      m_manager->releaseDbManager();

    return StatusCode::SUCCESS;
  }

  return StatusCode::FAILURE;
}

StatusCode TileDetectorTool::clear()
{
  SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<TileDetDescrManager>::ID(),m_manager->getName());
  if(proxy) {
    proxy->reset();
    m_manager = 0;
  }
  return StatusCode::SUCCESS;
}

StatusCode TileDetectorTool::initIds()
{
  const TileID* tileID;
  const CaloCell_ID* cellID;
  const TileHWID* tileHWID;

  // Retrieve all helpers from the detector store
  CHECK( detStore()->retrieve(tileID, "TileID") );
  CHECK( detStore()->retrieve(tileHWID, "TileHWID") );
  CHECK( detStore()->retrieve(cellID, "CaloCell_ID") );

  m_manager->set_helper(tileID);
  m_manager->set_helper(cellID);
  m_manager->set_helper(tileHWID);

  // instantiate Cabling Svc to initialize pointers to helpers there
  TileCablingService * cabling = TileCablingService::getInstance_nc();
  if(cabling==0)
  {
    ATH_MSG_ERROR("Could not get instance of TileCablingService");
    return StatusCode::FAILURE;
  }
  
  cabling->setTestBeam(m_switches.testBeam);

  return StatusCode::SUCCESS;
}


StatusCode TileDetectorTool::createElements()
{
  m_manager->create_elements(true);
  return StatusCode::SUCCESS;
}
