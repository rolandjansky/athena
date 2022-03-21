/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArReadoutGeometry/FCAL_ChannelMap.h"

#include "LArDetectorToolNV.h"
#include "LArDetectorFactory.h"
#include "LArDetectorFactoryLite.h"
#include "LArGeoCode/VDetectorParameters.h"
#include "LArGeoRAL/RAL.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelUtilities/StoredAlignX.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoGraphNode.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

#include "DetDescrConditions/DetCondKeyTrans.h"

#include "GeoModelUtilities/StoredPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelRead/ReadGeoModel.h"
#include "LArHV/LArHVManager.h"

#include "AthenaKernel/ClassID_traits.h"
#include "SGTools/DataProxy.h"

// Name of the folder for LAr Alignment information :
#define LAR_ALIGN "/LAR/Align"


LArDetectorToolNV::LArDetectorToolNV(const std::string& type
				     , const std::string& name
				     , const IInterface* parent)
  : GeoModelTool(type,name,parent)
  , m_barrelSaggingOn(false)
  , m_barrelVisLimit(-1)
  , m_fcalVisLimit(-1)
  , m_buildBarrel(true)
  , m_buildEndcap(true)
  , m_applyAlignments(false)
  , m_manager{nullptr}
  , m_geometryConfig("FULL")
  , m_EMECVariantInner("Wheel")
  , m_EMECVariantOuter("Wheel")
  , m_activateFT(true)
  , m_enableMBTS(true)
{
  declareProperty("SaggingBarrelAccordeon",m_barrelSaggingOn);
  declareProperty("BarrelCellVisLimit",    m_barrelVisLimit);
  declareProperty("FCALVisLimit",          m_fcalVisLimit);

  declareProperty("BuildBarrel",           m_buildBarrel);
  declareProperty("BuildEndcap",           m_buildEndcap);
  declareProperty("ApplyAlignments",       m_applyAlignments);
  declareProperty("GeometryConfig",        m_geometryConfig);
  declareProperty("EMECVariantInner",      m_EMECVariantInner);
  declareProperty("EMECVariantOuter",      m_EMECVariantOuter);
  declareProperty("ActivateFeedThrougs",   m_activateFT);
  declareProperty("EnableMBTS",            m_enableMBTS);
}

LArDetectorToolNV::~LArDetectorToolNV()
{
}

StatusCode LArDetectorToolNV::create()
{
  if(m_detector) {
    ATH_MSG_FATAL("LAr GeoModel description has already been built. LArDetectorToolNV::create() cannot be called more than once!");
    return StatusCode::FAILURE;
  }

  // Initialize the HV System:
  LArHVManager* hvManager = new LArHVManager();;
  ATH_CHECK(detStore()->record(hvManager,"LArHVManager"));

  // Get the detector configuration.
  ServiceHandle<IGeoDbTagSvc> geoDbTag("GeoDbTagSvc",name());
  ATH_CHECK(geoDbTag.retrieve());

  ServiceHandle<IRDBAccessSvc> accessSvc(geoDbTag->getParamSvcName(),name());
  ATH_CHECK(accessSvc.retrieve());

  GeoModelIO::ReadGeoModel* sqliteReader = geoDbTag->getSqliteReader();
  std::string detectorKey{""};
  std::string detectorNode{""};

  if(!sqliteReader) {
    // Geometry is constructed from the Geometry DB
    std::string AtlasVersion = geoDbTag->atlasVersion();
    std::string LArVersion   = geoDbTag->LAr_VersionOverride();

    detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
    detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";
    ATH_MSG_INFO("Keys for LAr are "  << detectorKey  << "  " << detectorNode);
    ATH_MSG_INFO("Building LAr version " << geoDbTag->LAr_Version()
		 << " while ATLAS version is " <<  AtlasVersion);
  }

  IRDBRecordset_ptr switchSet = accessSvc->getRecordsetPtr("LArSwitches", detectorKey, detectorNode);
  if ((*switchSet).size()==0) {
    ATH_MSG_ERROR("Unable to retrieve LArSwitches from Geometry DB");
    return StatusCode::FAILURE;
  }

  const IRDBRecord* switches = (*switchSet)[0];
  m_barrelSaggingOn = switches->getInt("SAGGING");
  
  if (!switches->isFieldNull("BARREL_ON")) {
    m_buildBarrel = switches->getInt("BARREL_ON");
  }

  if (!switches->isFieldNull("ENDCAP_ON")) {
    m_buildEndcap = switches->getInt("ENDCAP_ON");
  }

  ATH_MSG_INFO("LAr Geometry Options:"  );
  ATH_MSG_INFO("  Sagging           = "  << (m_barrelSaggingOn ? "true" : "false"));
  ATH_MSG_INFO("  Barrel            = "  << (m_buildBarrel ? "ON" : "OFF"));
  ATH_MSG_INFO("  Endcap            = "  << (m_buildEndcap ? "ON" : "OFF"));

  // Locate the top level experiment node
  GeoModelExperiment* theExpt = nullptr;
  ATH_CHECK(detStore()->retrieve(theExpt,"ATLAS"));

  // testbeam=0  Atlas
  int testbeam = 0;

  GeoPhysVol *world=theExpt->getPhysVol();
  if(sqliteReader) {
    // Geometry is constructed from SQLite file
    LArGeo::LArDetectorFactoryLite theLArFactoryLite(detStore().operator->()
						     , accessSvc.operator->()
						     , sqliteReader
						     , hvManager);
    theLArFactoryLite.setBarrelSagging(m_barrelSaggingOn);
    theLArFactoryLite.create(world);
    m_manager = theLArFactoryLite.getDetectorManager();

    std::unique_ptr<LArGeo::VDetectorParameters> params = std::make_unique<LArGeo::RAL>();
    LArGeo::VDetectorParameters::SetInstance(std::move(params));
  }
  else {
    // Geometry is constructed from the Geometry DB

    // testbeam=1  H8 test beam => build GeoModel in LArDetectorFactory
    // testbeam=2  H6 test beam => don't build GeoModel in LArDetectorFactory
    std::string LArTag = accessSvc->getChildTag("LAr",detectorKey,detectorNode);
    if(LArTag.find("H8")!=std::string::npos) {
      testbeam = 1 ;
    }
    else if(LArTag.find("H6")!=std::string::npos) {
      testbeam = 2 ;
    }

    LArGeo::LArDetectorFactory theLArFactory(detStore().operator->()
					     , hvManager
					     , testbeam
					     , m_geometryConfig=="FULL");

    theLArFactory.setBarrelSagging       (m_barrelSaggingOn);
    theLArFactory.setBarrelCellVisLimit  (m_barrelVisLimit);
    theLArFactory.setFCALVisLimit        (m_fcalVisLimit);
    theLArFactory.setBuildBarrel(m_buildBarrel);
    theLArFactory.setBuildEndcap(m_buildEndcap);
    theLArFactory.setEMECVariant(m_EMECVariantInner, m_EMECVariantOuter);
    theLArFactory.setActivateFT(m_activateFT);
    theLArFactory.setEnableMBTS(m_enableMBTS);

    theLArFactory.create(world);
    m_manager = theLArFactory.getDetectorManager();
    if(m_geometryConfig!="RECO") {
      // Save RDB Recordsets if we are not inside reco job
      LArGeo::VDetectorParameters::SetInstance(theLArFactory.moveParameters());
    }
  }

  ATH_CHECK(detStore()->record(m_manager,m_manager->getName()));
  theExpt->addManager(m_manager);

  return StatusCode::SUCCESS;
}

StatusCode LArDetectorToolNV::clear()
{
  StoredPhysVol* storedPV{nullptr};
  StatusCode status(StatusCode::SUCCESS);
  std::vector<std::string> sgkeysFPV = detStore()->keys<StoredPhysVol>();
  // First loop over all Full Physical volumes and initialize their global position informations
  for (const std::string& stored : sgkeysFPV) {
    status = detStore()->retrieve(storedPV,stored);
    if(status.isFailure()) {
      storedPV = nullptr;
      continue;
    }

    if(storedPV) {
      storedPV->getPhysVol()->getAbsoluteTransform();
      storedPV->getPhysVol()->getDefAbsoluteTransform();
    }
  }

  for (const std::string& stored : sgkeysFPV) {
    status = detStore()->retrieve(storedPV,stored);
    if(status.isFailure()) {
      storedPV = nullptr;
      continue;
    }

    if(storedPV) {
      storedPV->getPhysVol()->clear();
    }
  }

  // Release all Stored XF and Stored FPV from the detector store
  std::vector<std::string> sgkeysAXF = detStore()->keys<StoredAlignX>();
  for (const std::string& stored : sgkeysAXF) {
    SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<StoredAlignX>::ID(),stored);
    if(proxy) proxy->reset();
  }

  // Release manager from the detector store
  SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<LArDetectorManager>::ID(),m_manager->getName());
  if(proxy) {
    proxy->reset();
    m_manager = nullptr;
  }

  return StatusCode::SUCCESS;
}

StatusCode LArDetectorToolNV::registerCallback()
{
  // Return FAILURE if no callbacks have been registered
  if(!m_applyAlignments) {
    ATH_MSG_DEBUG("LAr alignments switched OFF");
    return StatusCode::FAILURE;
  }

  const DataHandle<DetCondKeyTrans> dckt;
  StatusCode sc = detStore()->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool *>(this), dckt, LAR_ALIGN);
  ATH_MSG_DEBUG( (sc.isSuccess() ? "Successfully registered" : "Registration failed for")
		 << " callback on DetCondKeyTrans with folder " << LAR_ALIGN);

  return sc;
}

StatusCode LArDetectorToolNV::align(IOVSVC_CALLBACK_ARGS)
{
  if(!m_applyAlignments) {
    ATH_MSG_DEBUG("LAr alignments switched OFF");
    return StatusCode::SUCCESS;
  }

  if(m_manager==nullptr) {
    ATH_MSG_ERROR(" LArDetDescrManager not created yet, cannot align !");
    return StatusCode::FAILURE;
  }

  if(detStore()->contains<DetCondKeyTrans>(LAR_ALIGN)) {
    const DetCondKeyTrans* align{nullptr};
    StatusCode sc = detStore()->retrieve(align, LAR_ALIGN);

    if(sc.isFailure()) {
      ATH_MSG_ERROR(" Could not retrieve LAr DetCondKeyTrans ");
      return sc;
    }

    if(align==nullptr) {
      ATH_MSG_ERROR("LAr DetCondKeyTrans ptr is null");
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG(" LAr DetCondKeyTrans retrieved ");
    if(m_alignHelper.applyAlignments(detStore(),align,nullptr).isFailure()) {
      ATH_MSG_ERROR("Failed to apply LAr alignments to GeoModel");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("LAr Alignments applied successfully");
  }
  else {
    ATH_MSG_DEBUG(" No LAr DetCondKeyTrans in SG, skipping align() ");
  }

  return StatusCode::SUCCESS;
}

