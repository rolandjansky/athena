/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  m_testBeam(false),
  m_addPlates(true),
  m_uShape(-1),
  m_glue(-1),
  m_csTube(-1),
  m_not_locked(true),
  m_useNewFactory(true),
  m_geometryConfig("FULL"),
  m_manager(0)
{
  declareProperty("TestBeam", m_testBeam);
  //  declareProperty("AddPlatesToCellVolume", m_addPlates); to make user aware that the value is taken from DB rather from JO
  declareProperty("UseNewFactory", m_useNewFactory);
  declareProperty("GeometryConfig",m_geometryConfig);
  declareProperty("Ushape",m_uShape);
  declareProperty("Glue",m_glue);
  declareProperty("CsTube",m_csTube);
}

TileDetectorTool::~TileDetectorTool()
{
}

StatusCode TileDetectorTool::create()
{ 
  MsgStream log(msgSvc(), name());
  ATH_MSG_INFO(" Entering TileDetectorTool::create()");

  // Get the detector configuration
  IGeoModelSvc *geoModel;
  service ("GeoModelSvc",geoModel);

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
    m_testBeam = true;
  }
  
  //Locate the top level experiment node
  GeoModelExperiment* theExpt = nullptr;
  CHECK( detStore()->retrieve(theExpt, "ATLAS") );

  if ( 0 == m_detector )
  {
    IRDBAccessSvc* raccess = 0;
    service("RDBAccessSvc",raccess);

    TileDddbManager_ptr  dbManager(new TileDddbManager(raccess,versionTag,versionNode));
    m_manager = new TileDetDescrManager(dbManager);

    if (0==dbManager->GetNumberOfEnv() && m_useNewFactory) {
      ATH_MSG_WARNING("New TileAtlasFactory can not be used because TileGlobals do not exist in Database");
      ATH_MSG_WARNING("Use old TileDetectorFactory instead");
      m_useNewFactory = false;
    }

    CHECK( initIds() );

    int uShapeDB = dbManager->uShape();
    if (m_uShape < 0) {
      m_uShape = uShapeDB;
      ATH_MSG_INFO(" U-shape parameter from database is: " << m_uShape);
    } else {
      if (m_uShape != uShapeDB) {
        ATH_MSG_WARNING(" Overriding U-shape value from DB by value from jobOptions, using "
                        << m_uShape << " instead of " << uShapeDB);
      } else {
        ATH_MSG_INFO(" U-shape parameter from jobOptions is: " << m_uShape);
      }
    }

    int glueDB = dbManager->glue();
    if (m_glue < 0) {
      m_glue = glueDB;
      ATH_MSG_INFO(" Glue parameter from database is: " << m_glue);
    } else {
      if (m_glue != glueDB) {
        ATH_MSG_WARNING(" Overriding Glue value from DB by value from jobOptions, using "
                        << m_glue << " instead of " << glueDB);
      } else {
        ATH_MSG_INFO(" Glue parameter from jobOptions is: " << m_glue);
      }
    }

    int csTubeDB = 0;
    if (m_csTube < 0) {
       m_csTube = csTubeDB;
       ATH_MSG_INFO(" Cs Tube parameter from database is: " << m_csTube);
    } else {
       if (m_csTube != csTubeDB) {
           ATH_MSG_WARNING(" Overriding U-shape value from DB by value from jobOptions, using "
               << m_csTube << " instead of " << csTubeDB);
       } else {
           ATH_MSG_INFO(" Cs Tube parameter from jobOptions is: " << m_csTube);
       }
    }

    m_not_locked = false;
    
    m_addPlates = dbManager->addPlatesToCell();
    GeoPhysVol *world=&*theExpt->getPhysVol();
    if(m_testBeam)
    {
      TileTBFactory theTileTBFactory(detStore().operator->(),m_manager,m_addPlates,m_uShape,m_glue,m_csTube,&log);
      theTileTBFactory.create(world);
    }
    else if (m_useNewFactory)
    {
      TileAtlasFactory theTileFactory(detStore().operator->(),m_manager,m_addPlates,m_uShape,m_glue,m_csTube,&log,m_geometryConfig=="FULL");
      theTileFactory.create(world);
    }
    else
    {
      TileDetectorFactory theTileFactory(detStore().operator->(),m_manager,m_addPlates,m_uShape,m_glue,m_csTube,&log);
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

  // instanciate Cabling Svc to initialize pointers to helpers there
  TileCablingService * cabling = TileCablingService::getInstance();
  if(cabling==0)
  {
    ATH_MSG_ERROR("Could not get instance of TileCablingService");
    return StatusCode::FAILURE;
  }
  
  cabling->setTestBeam(m_testBeam);

  return StatusCode::SUCCESS;
}


StatusCode TileDetectorTool::createElements()
{
  m_manager->create_elements(true);
  return StatusCode::SUCCESS;
}
