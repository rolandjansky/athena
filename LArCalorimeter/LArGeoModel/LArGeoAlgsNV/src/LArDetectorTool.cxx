/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArReadoutGeometry/FCAL_ChannelMap.h"

#include "LArGeoAlgsNV/LArDetectorToolNV.h"
#include "LArGeoAlgsNV/LArDetectorFactory.h"
#include "LArGeoCode/VDetectorParameters.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/StoredAlignX.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoGraphNode.h"

#include "DetDescrConditions/DetCondKeyTrans.h"

#include "GeoModelUtilities/StoredPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "LArHV/LArHVManager.h"
#include "LArHV/EMBHVManager.h"
#include "LArHV/EMECHVManager.h"
#include "LArHV/HECHVManager.h"
#include "LArHV/FCALHVManager.h"
#include "LArHV/EMBPresamplerHVManager.h"
#include "LArHV/EMECPresamplerHVManager.h"

#include "CLIDSvc/tools/ClassID_traits.h"
#include "SGTools/DataProxy.h"

// Name of the folder for LAr Alignment information :
#define LAR_ALIGN "/LAR/Align"


LArDetectorToolNV::LArDetectorToolNV(const std::string& type,
				     const std::string& name,
				     const IInterface* parent)
  : GeoModelTool(type,name,parent),
    m_barrelSaggingOn(false),
    m_barrelVisLimit(-1),
    m_fcalVisLimit(-1),
    m_buildBarrel(true),
    m_buildEndcap(true),
    m_applyAlignments(false),
    m_detStore(0),
    m_geometryConfig("FULL"), m_activateFT(false)
{
  declareProperty("SaggingBarrelAccordeon",m_barrelSaggingOn);
  declareProperty("BarrelCellVisLimit",    m_barrelVisLimit);
  declareProperty("FCALVisLimit",          m_fcalVisLimit);

  declareProperty("BuildBarrel",           m_buildBarrel);
  declareProperty("BuildEndcap",           m_buildEndcap);
  declareProperty("ApplyAlignments",       m_applyAlignments);
  declareProperty("GeometryConfig",        m_geometryConfig);
  declareProperty("ActivateFeedThrougs",   m_activateFT);
  m_manager = 0;

}

LArDetectorToolNV::~LArDetectorToolNV()
{
  // Clean up detector parameters instantiated by the factory
  LArGeo::VDetectorParameters::SetInstance(0);
}

StatusCode LArDetectorToolNV::create(StoreGateSvc* detStore)
{
  IMessageSvc* msgSvc;
  if(StatusCode::FAILURE==service ("MessageSvc",msgSvc))
  {
    std::cerr << "LArDetectorToolNV::create ERROR unable to get message service!\n";
    return StatusCode::FAILURE;
  }

  // Initialize the HV System:
  const EMBHVManager *embHV=new EMBHVManager();
  const EMECHVManager *emecHVI= new EMECHVManager(EMECHVModule::INNER);
  const EMECHVManager *emecHVO= new EMECHVManager(EMECHVModule::OUTER);
  const HECHVManager *hecHV   = new HECHVManager();
  const FCALHVManager *fcalHV= new FCALHVManager();
  const EMBPresamplerHVManager *embPSHV = new EMBPresamplerHVManager();
  const EMECPresamplerHVManager *emecPSHV = new EMECPresamplerHVManager();


  LArHVManager *hvManager= new LArHVManager(embHV,emecHVI, emecHVO, hecHV, fcalHV,embPSHV, emecPSHV);


  MsgStream log(msgSvc, name());
  m_detStore = detStore;

  if (StatusCode::SUCCESS != detStore->record(hvManager,"LArHVManager")) {
     log << MSG::ERROR << "Unable to record LArHVManager in detector store " << endreq;
     return StatusCode::FAILURE;
  }


  // Get the detector configuration.
  IGeoModelSvc *geoModel;
  if(StatusCode::FAILURE==service ("GeoModelSvc",geoModel))
  {
    log << MSG::ERROR << "Unable to get GeoModel service" <<  endreq;
    return StatusCode::FAILURE;
  }

  std::string AtlasVersion = geoModel->atlasVersion();
  std::string LArVersion   = geoModel->LAr_VersionOverride();

  IRDBAccessSvc *accessSvc;
  if(StatusCode::FAILURE==service("RDBAccessSvc",accessSvc))
  {
    log << MSG::ERROR << "Unable to get RDBAccess service" <<  endreq;
    return StatusCode::FAILURE;
  }

  std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
  std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";
  log << MSG::INFO << "Keys for LAr are "  << detectorKey  << "  " << detectorNode << endreq;
  log << MSG::INFO << "Building LAr version " << geoModel->LAr_Version()
      << " while ATLAS version is " <<  AtlasVersion << endreq;

  if(LArVersion=="CUSTOM")
  {
    log << MSG::WARNING << "LArDetectorToolNV:  Detector Information coming from a custom configuration!!"
	<< endreq;
  }
  else
  {
    IRDBRecordset_ptr switchSet = accessSvc->getRecordsetPtr("LArSwitches", detectorKey, detectorNode);
    if ((*switchSet).size()==0) return StatusCode::FAILURE;
    const IRDBRecord    *switches   = (*switchSet)[0];

    m_barrelSaggingOn           = switches->getInt("SAGGING");

    try
    {
      if (!switches->isFieldNull("BARREL_ON"))
	m_buildBarrel = switches->getInt("BARREL_ON");

      if (!switches->isFieldNull("ENDCAP_ON"))
	m_buildEndcap = switches->getInt("ENDCAP_ON");
    }
    catch(std::exception& e)
    {
      log << MSG::DEBUG << e.what() << endreq;
    }
  }

  log << MSG::INFO  << "LAr Geometry Options:"   << endreq;
  log << MSG::INFO  << "  Sagging           = "  << (m_barrelSaggingOn ? "true" : "false") << endreq;
  log << MSG::INFO  << "  Barrel            = "  << (m_buildBarrel ? "ON" : "OFF") << endreq;
  log << MSG::INFO  << "  Endcap            = "  << (m_buildEndcap ? "ON" : "OFF") << endreq;

  // Locate the top level experiment node
  DataHandle<GeoModelExperiment> theExpt;
  if (StatusCode::SUCCESS != detStore->retrieve( theExpt, "ATLAS" ))
  {
    log << MSG::ERROR << "Could not find GeoModelExperiment ATLAS" << endreq;
    return (StatusCode::FAILURE);
  }

  // determine the geometry layout - Atlas/Testbeam
  std::string geometryLayout = "Atlas";
  int testbeam = 0 ;
  // testbeam=0  Atlas
  // testbeam=1  H8 test beam => build GeoModel in LArDetectorFactory
  // testbeam=2  H6 test beam => don't build GeoModel in LArDetectorFactory


  std::string LArTag = accessSvc->getChildTag("LAr",detectorKey,detectorNode);

  if(LArTag.find("H8")!=std::string::npos) {
    geometryLayout = "H8";
    testbeam = 1 ;
  }
  else if(LArTag.find("H6")!=std::string::npos) {
    geometryLayout = "H6";
    testbeam = 2 ;
  }
  else if(LArTag.find("G3")!=std::string::npos)
    geometryLayout = "G3";

  LArGeo::LArDetectorFactory theLArFactory(testbeam,m_geometryConfig=="FULL");

  theLArFactory.setBarrelSagging       (m_barrelSaggingOn);
  theLArFactory.setBarrelCellVisLimit  (m_barrelVisLimit);
  theLArFactory.setFCALVisLimit        (m_fcalVisLimit);
  theLArFactory.setBuildBarrel(m_buildBarrel);
  theLArFactory.setBuildEndcap(m_buildEndcap);
  theLArFactory.setActivateFT(m_activateFT);


  if (0 == m_detector)
  {
    GeoPhysVol *world=&*theExpt->getPhysVol();
    theLArFactory.create(world);
    m_manager = theLArFactory.getDetectorManager();
    if (StatusCode::SUCCESS != detStore->record(theLArFactory.getDetectorManager(),
						theLArFactory.getDetectorManager()->getName()))
      {
	log << MSG::ERROR << "Could not record" << endreq;
	return (StatusCode::FAILURE);
      }

    theExpt->addManager(theLArFactory.getDetectorManager());

    // Release RDB Recordsets if we are inside reco job
    if(m_geometryConfig=="RECO")
      LArGeo::VDetectorParameters::SetInstance(0);

    return StatusCode::SUCCESS;
  }


  return StatusCode::FAILURE;
}

StatusCode LArDetectorToolNV::clear(StoreGateSvc* detStore)
{
  std::vector<std::string>::const_iterator itStored;

  StoredPhysVol* storedPV(0);
  StatusCode status(StatusCode::SUCCESS);
  std::vector<std::string> sgkeysFPV = detStore->keys<StoredPhysVol>();
  // First loop over all Full Physical volumes and initialize their global position informations
  for(itStored=sgkeysFPV.begin();itStored!=sgkeysFPV.end();itStored++) {
    status = detStore->retrieve(storedPV,*itStored);
    if(status.isFailure()) {
      storedPV = 0;
      continue;
    }

    if(storedPV) {
      storedPV->getPhysVol()->getAbsoluteTransform();
      storedPV->getPhysVol()->getDefAbsoluteTransform();
    }
  }

  for(itStored=sgkeysFPV.begin();itStored!=sgkeysFPV.end();itStored++) {
    status = detStore->retrieve(storedPV,*itStored);
    if(status.isFailure()) {
      storedPV = 0;
      continue;
    }

    if(storedPV) {
      storedPV->getPhysVol()->clear();
    }
  }

  // Release all Stored XF and Stored FPV from the detector store
  std::vector<std::string> sgkeysAXF = detStore->keys<StoredAlignX>();
  for(itStored=sgkeysAXF.begin();itStored!=sgkeysAXF.end();itStored++) {
    SG::DataProxy* proxy = detStore->proxy(ClassID_traits<StoredAlignX>::ID(),*itStored);
    if(proxy)
      proxy->reset();
  }

  // Release manager from the detector store
  SG::DataProxy* proxy = detStore->proxy(ClassID_traits<LArDetectorManager>::ID(),m_manager->getName());
  if(proxy) {
    proxy->reset();
    m_manager = 0;
  }

  return StatusCode::SUCCESS;
}

StatusCode LArDetectorToolNV::registerCallback(StoreGateSvc* detStore)
{
  // Return FAILURE if no callbacks have been registered
  MsgStream log(msgSvc(), name());

  if(!m_applyAlignments)
  {
    log << MSG::DEBUG << "LAr alignments switched OFF" << endreq;
    return false;
  }

  std::string folderName = "/LAR/Align";

  const DataHandle<DetCondKeyTrans> dckt;
  log << MSG::DEBUG << "Registering callback on DetCondKeyTrans with folder " << folderName << endreq;
  StatusCode sc = detStore->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool *>(this), dckt, folderName);
  if(sc.isSuccess())
    log << MSG::DEBUG << " Successfully registered " << endreq;
  else
    log << MSG::DEBUG << " Registration failed " << endreq;

  return sc;
}

StatusCode LArDetectorToolNV::align(IOVSVC_CALLBACK_ARGS)
{
  MsgStream log(msgSvc(), name());

  if(!m_applyAlignments)
  {
    log << MSG::DEBUG << "LAr alignments switched OFF" << endreq;
    return StatusCode::SUCCESS;
  }

  // !!! To Do --->
  // Find a better solution for this

  std::vector< std::string > alignNames;
  alignNames.push_back("LARCRYO_B");
  alignNames.push_back("LARCRYO_EC_POS");
  alignNames.push_back("LARCRYO_EC_NEG");
  alignNames.push_back("PRESAMPLER_B_POS");
  alignNames.push_back("PRESAMPLER_B_NEG");
  alignNames.push_back("EMB_POS");
  alignNames.push_back("EMB_NEG");
  alignNames.push_back("PRESAMPLER_EC_POS");
  alignNames.push_back("PRESAMPLER_EC_NEG");
  alignNames.push_back("EMEC_POS");
  alignNames.push_back("EMEC_NEG");
  alignNames.push_back("HEC_POS");  // Policy:  either HEC_POS is present or HEC1_POS and HEC2_POS
  alignNames.push_back("HEC_NEG");  // Same with HEC_NEG.  Now in recent releases if HEC_POS is found
  alignNames.push_back("HEC1_POS"); // it will be applied to both HEC1 and HEC2...
  alignNames.push_back("HEC1_NEG");
  alignNames.push_back("HEC2_POS");
  alignNames.push_back("HEC2_NEG");
  alignNames.push_back("FCAL1_POS");
  alignNames.push_back("FCAL1_NEG");
  alignNames.push_back("FCAL2_POS");
  alignNames.push_back("FCAL2_NEG");
  alignNames.push_back("FCAL3_POS");
  alignNames.push_back("FCAL3_NEG");
  alignNames.push_back("SOLENOID");

  // <--- !!! To Do

  if(0 == m_manager)
  {
    log << MSG::WARNING << " LArDetDescrManager not created yet, cannot align !" << endreq;
    return StatusCode::FAILURE;
  }

  const DetCondKeyTrans* align=0;
  if(m_detStore->contains<DetCondKeyTrans>(LAR_ALIGN))
  {
    StatusCode sc = m_detStore->retrieve(align, LAR_ALIGN);

    if(sc.isFailure())
    {
      log << MSG::WARNING << " Could not retrieve LAr DetCondKeyTrans " << endreq;
      return sc;
    }

    if(0 == align)
    {
      log << MSG::WARNING <<" LAr DetCondKeyTrans ptr is 0" << endreq;
      return StatusCode::FAILURE;
    }

    log << MSG::DEBUG << " LAr DetCondKeyTrans retrieved " << endreq;

    // Special treatment for the HEC:
    StoredAlignX *hec1AlxPos=0;
    StoredAlignX *hec2AlxPos=0;
    StoredAlignX *hec1AlxNeg=0;
    StoredAlignX *hec2AlxNeg=0;
    if (m_detStore->contains<StoredAlignX> ("HEC1_POS")) {
      if (m_detStore->retrieve(hec1AlxPos,"HEC1_POS")!=StatusCode::SUCCESS) {
	log << MSG::WARNING << " Unable to retrieve StoredAlignX for the key HEC1_POS" << endreq;
      }
    }
    if (m_detStore->contains<StoredAlignX> ("HEC1_NEG")) {
      if (m_detStore->retrieve(hec1AlxNeg,"HEC1_NEG")!=StatusCode::SUCCESS) {
	log << MSG::WARNING << " Unable to retrieve StoredAlignX for the key HEC1_NEG" << endreq;
      }
    }
    if (m_detStore->contains<StoredAlignX> ("HEC2_POS")) {
      if (m_detStore->retrieve(hec2AlxPos,"HEC2_POS")!=StatusCode::SUCCESS) {
	log << MSG::WARNING << " Unable to retrieve StoredAlignX for the key HEC2_POS" << endreq;
      }
    }
    if (m_detStore->contains<StoredAlignX> ("HEC2_NEG")) {
      if (m_detStore->retrieve(hec2AlxNeg,"HEC2_NEG")!=StatusCode::SUCCESS) {
	log << MSG::WARNING << " Unable to retrieve StoredAlignX for the key HEC2_NEG" << endreq;
      }
    }
    GeoAlignableTransform *hec1GatPos = hec1AlxPos ? hec1AlxPos->getAlignX(): NULL;
    GeoAlignableTransform *hec1GatNeg = hec1AlxPos ? hec1AlxNeg->getAlignX(): NULL;
    GeoAlignableTransform *hec2GatPos = hec2AlxPos ? hec2AlxPos->getAlignX(): NULL;
    GeoAlignableTransform *hec2GatNeg = hec2AlxPos ? hec2AlxNeg->getAlignX(): NULL;

    // loop over align names
    // if the transform presented alter its delta
    // if the transform is not presented clear its delta
    for(unsigned int i=0; i<alignNames.size(); i++)
    {
      std::string alignName = alignNames[i];
      // First try to retrieve StoredAlignX
      if(m_detStore->contains<StoredAlignX>(alignName))
      {
	StoredAlignX* alignX = 0;
	sc = m_detStore->retrieve(alignX,alignName);

	if(sc.isFailure())
	{
	  log << MSG::WARNING << " Unable to retrieve StoredAlignX for the key " << alignName << endreq;
	  return sc;
	}

	if(!alignX)
	{
	  log << MSG::WARNING << " 0 pointer to StoredAlignX for the key " << alignName << endreq;
	  return StatusCode::FAILURE;
	}

	GeoAlignableTransform* gat = alignX->getAlignX();
	if(!gat)
	{
	  log << MSG::WARNING << " 0 pointer to GeoAlignableTransform for the key " << alignName << endreq;
	  return StatusCode::FAILURE;
	}

	// check existence of new delta in DetCondKeyTrans
	HepGeom::Transform3D newDelta;
	if(align->getTransform(alignName,newDelta))
	  gat->setDelta(newDelta);
	else
	  gat->clearDelta();
      }
      else if (alignName=="HEC_POS") {
	if (hec1GatPos) {
	  HepGeom::Transform3D newDelta;
	  if(align->getTransform(alignName,newDelta))
	    hec1GatPos->setDelta(newDelta);
	  else
	    hec1GatPos->clearDelta();
	}
	if (hec2GatPos) {
	  HepGeom::Transform3D newDelta;
	  if(align->getTransform(alignName,newDelta))
	    hec2GatPos->setDelta(newDelta);
	  else
	    hec2GatPos->clearDelta();
	}
      }
      else if (alignName=="HEC_NEG") {
	if (hec1GatNeg) {
	  HepGeom::Transform3D newDelta;
	  if(align->getTransform(alignName,newDelta))
	    hec1GatNeg->setDelta(newDelta);
	  else
	    hec1GatNeg->clearDelta();
	}
	if (hec2GatNeg) {
	  HepGeom::Transform3D newDelta;
	  if(align->getTransform(alignName,newDelta))
	    hec2GatNeg->setDelta(newDelta);
	  else
	    hec2GatNeg->clearDelta();
	}
      }
      else
	log << MSG::DEBUG << " No StoredAlignX for the key " << alignName << endreq;
    } // for
  }
  else
    log << MSG::DEBUG << " No LAr DetCondKeyTrans in SG, skipping align() " << endreq;


  // debug printout of global positions:
  for(unsigned int i=0; i<alignNames.size(); i++)
  {
    std::string alignName = alignNames[i];
    if(m_detStore->contains<StoredPhysVol>(alignName))
    {
	StoredPhysVol* storedPV = 0;
	StatusCode sc = m_detStore->retrieve(storedPV,alignName);
	if(sc.isSuccess())
	{
	  const GeoFullPhysVol* fullPV = storedPV->getPhysVol();
	  const HepGeom::Transform3D& xf =  fullPV->getAbsoluteTransform();
	  CLHEP::Hep3Vector trans=xf.getTranslation();
	  CLHEP::HepRotation rot=xf.getRotation();
	  log << MSG::DEBUG << "Dump Absolute Transform:" << endreq;
	  log << MSG::DEBUG << "Key " << alignName << " transl [" << trans.x()
               << "," << trans.y() << "," << trans.z() << "] rotation ("
               << rot.phi() << "," << rot.theta() << "," <<  rot.psi() << ")"
               << endreq;
	}
    }
  }

  return StatusCode::SUCCESS;
}

