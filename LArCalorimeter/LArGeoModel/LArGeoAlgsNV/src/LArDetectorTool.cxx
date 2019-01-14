/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArReadoutGeometry/FCAL_ChannelMap.h"

#include "LArDetectorToolNV.h"
#include "LArDetectorFactory.h" 
#include "LArGeoCode/VDetectorParameters.h"
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
#include "LArHV/LArHVManager.h"
#include "LArHV/EMBHVManager.h"
#include "LArHV/EMECHVManager.h"
#include "LArHV/HECHVManager.h"
#include "LArHV/FCALHVManager.h"
#include "LArHV/EMBPresamplerHVManager.h"
#include "LArHV/EMECPresamplerHVManager.h"

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
{
  declareProperty("SaggingBarrelAccordeon",m_barrelSaggingOn);
  declareProperty("BarrelCellVisLimit",    m_barrelVisLimit);
  declareProperty("FCALVisLimit",          m_fcalVisLimit);

  declareProperty("BuildBarrel",           m_buildBarrel);
  declareProperty("BuildEndcap",           m_buildEndcap);
  declareProperty("ApplyAlignments",       m_applyAlignments);
  declareProperty("GeometryConfig",        m_geometryConfig);
}

LArDetectorToolNV::~LArDetectorToolNV()
{
  // Clean up detector parameters instantiated by the factory
  LArGeo::VDetectorParameters::SetInstance(0);
}

StatusCode LArDetectorToolNV::create()
{ 
  // Initialize the HV System:
  const EMBHVManager *embHV=new EMBHVManager();
  const EMECHVManager *emecHVI= new EMECHVManager(EMECHVModule::INNER);
  const EMECHVManager *emecHVO= new EMECHVManager(EMECHVModule::OUTER);
  const HECHVManager *hecHV   = new HECHVManager();
  const FCALHVManager *fcalHV= new FCALHVManager();
  const EMBPresamplerHVManager *embPSHV = new EMBPresamplerHVManager();
  const EMECPresamplerHVManager *emecPSHV = new EMECPresamplerHVManager();

  LArHVManager *hvManager= new LArHVManager(embHV,emecHVI, emecHVO, hecHV, fcalHV,embPSHV, emecPSHV);

  ATH_CHECK(detStore()->record(hvManager,"LArHVManager"));  

  // Get the detector configuration.
  ServiceHandle<IGeoDbTagSvc> geoDbTag("GeoDbTagSvc",name());
  ATH_CHECK(geoDbTag.retrieve());

  std::string AtlasVersion = geoDbTag->atlasVersion();
  std::string LArVersion   = geoDbTag->LAr_VersionOverride();

  ServiceHandle<IRDBAccessSvc> accessSvc("RDBAccessSvc",name());
  ATH_CHECK(accessSvc.retrieve());

  std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
  std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";
  ATH_MSG_INFO("Keys for LAr are "  << detectorKey  << "  " << detectorNode);
  ATH_MSG_INFO("Building LAr version " << geoDbTag->LAr_Version()
      << " while ATLAS version is " <<  AtlasVersion);

  if(LArVersion=="CUSTOM") {
    ATH_MSG_WARNING("LArDetectorToolNV:  Detector Information coming from a custom configuration!!");
  } 
  else {
    IRDBRecordset_ptr switchSet = accessSvc->getRecordsetPtr("LArSwitches", detectorKey, detectorNode);
    if ((*switchSet).size()==0) {
      ATH_MSG_ERROR("Unable to retrieve LArSwitches from Geometry DB");
      return StatusCode::FAILURE;
    }
    const IRDBRecord    *switches   = (*switchSet)[0];
    m_barrelSaggingOn           = switches->getInt("SAGGING");

    try {
      if (!switches->isFieldNull("BARREL_ON")) {
	m_buildBarrel = switches->getInt("BARREL_ON");
      }
      
      if (!switches->isFieldNull("ENDCAP_ON")) {
	m_buildEndcap = switches->getInt("ENDCAP_ON");
      }
    }
    catch(const std::exception& e) {
      ATH_MSG_DEBUG(e.what());
    }
  }

  ATH_MSG_INFO("LAr Geometry Options:"  );
  ATH_MSG_INFO("  Sagging           = "  << (m_barrelSaggingOn ? "true" : "false"));
  ATH_MSG_INFO("  Barrel            = "  << (m_buildBarrel ? "ON" : "OFF"));
  ATH_MSG_INFO("  Endcap            = "  << (m_buildEndcap ? "ON" : "OFF"));

  // Locate the top level experiment node 
  GeoModelExperiment* theExpt = nullptr;
  ATH_CHECK(detStore()->retrieve(theExpt,"ATLAS"));

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
  else if(LArTag.find("G3")!=std::string::npos) {
    geometryLayout = "G3";
  }

  LArGeo::LArDetectorFactory theLArFactory(testbeam,m_geometryConfig=="FULL");

  theLArFactory.setBarrelSagging       (m_barrelSaggingOn);
  theLArFactory.setBarrelCellVisLimit  (m_barrelVisLimit);
  theLArFactory.setFCALVisLimit        (m_fcalVisLimit);
  theLArFactory.setBuildBarrel(m_buildBarrel);
  theLArFactory.setBuildEndcap(m_buildEndcap);

  if(m_detector==nullptr) {
    GeoPhysVol *world=&*theExpt->getPhysVol();
    theLArFactory.create(world);
    m_manager = theLArFactory.getDetectorManager();
    ATH_CHECK(detStore()->record(theLArFactory.getDetectorManager(),theLArFactory.getDetectorManager()->getName()));
    theExpt->addManager(theLArFactory.getDetectorManager());

    // Release RDB Recordsets if we are inside reco job
    if(m_geometryConfig=="RECO") LArGeo::VDetectorParameters::SetInstance(0);
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode LArDetectorToolNV::clear()
{
  std::vector<std::string>::const_iterator itStored;

  StoredPhysVol* storedPV{nullptr};
  StatusCode status(StatusCode::SUCCESS);
  std::vector<std::string> sgkeysFPV = detStore()->keys<StoredPhysVol>();
  // First loop over all Full Physical volumes and initialize their global position informations
  for(itStored=sgkeysFPV.begin();itStored!=sgkeysFPV.end();itStored++) {
    status = detStore()->retrieve(storedPV,*itStored);
    if(status.isFailure()) {
      storedPV = nullptr;
      continue;
    }

    if(storedPV) {
      storedPV->getPhysVol()->getAbsoluteTransform();
      storedPV->getPhysVol()->getDefAbsoluteTransform();
    }
  }

  for(itStored=sgkeysFPV.begin();itStored!=sgkeysFPV.end();itStored++) {
    status = detStore()->retrieve(storedPV,*itStored);
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
  for(itStored=sgkeysAXF.begin();itStored!=sgkeysAXF.end();itStored++) {
    SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<StoredAlignX>::ID(),*itStored);
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

  std::string folderName = "/LAR/Align";

  const DataHandle<DetCondKeyTrans> dckt;
  ATH_MSG_DEBUG("Registering callback on DetCondKeyTrans with folder " << folderName);
  StatusCode sc = detStore()->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool *>(this), dckt, folderName); 
  if(sc.isSuccess()) {
    ATH_MSG_DEBUG(" Successfully registered ");
  }
  else {
    ATH_MSG_DEBUG(" Registration failed ");
  }

  return sc;
}

StatusCode LArDetectorToolNV::align(IOVSVC_CALLBACK_ARGS)
{
  if(!m_applyAlignments) {
    ATH_MSG_DEBUG("LAr alignments switched OFF");
    return StatusCode::SUCCESS;
  }

  std::vector<std::string> alignNames {
    "LARCRYO_B"
      ,"LARCRYO_EC_POS"
      ,"LARCRYO_EC_NEG"
      ,"PRESAMPLER_B_POS"
      ,"PRESAMPLER_B_NEG"
      ,"EMB_POS"
      ,"EMB_NEG"
      ,"PRESAMPLER_EC_POS"
      ,"PRESAMPLER_EC_NEG"
      ,"EMEC_POS"
      ,"EMEC_NEG"
      ,"HEC_POS"  // Policy:  either HEC_POS is present or HEC1_POS and HEC2_POS
      ,"HEC_NEG"  // Same with HEC_NEG.  Now in recent releases if HEC_POS is found
      ,"HEC1_POS" // it will be applied to both HEC1 and HEC2...
      ,"HEC1_NEG"
      ,"HEC2_POS"
      ,"HEC2_NEG"
      ,"FCAL1_POS"
      ,"FCAL1_NEG"
      ,"FCAL2_POS"
      ,"FCAL2_NEG"
      ,"FCAL3_POS"
      ,"FCAL3_NEG"
      ,"SOLENOID"
      };

  if(m_manager==nullptr) {
    ATH_MSG_ERROR(" LArDetDescrManager not created yet, cannot align !");
    return StatusCode::FAILURE;
  }

  const DetCondKeyTrans* align=0;
  if(detStore()->contains<DetCondKeyTrans>(LAR_ALIGN)) {
    StatusCode sc = detStore()->retrieve(align, LAR_ALIGN);
  
    if(sc.isFailure()) {
      ATH_MSG_ERROR(" Could not retrieve LAr DetCondKeyTrans ");
      return sc;
    }

    if(0 == align) {
      ATH_MSG_ERROR(" LAr DetCondKeyTrans ptr is 0");
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG(" LAr DetCondKeyTrans retrieved ");

    // Special treatment for the HEC:
    StoredAlignX *hec1AlxPos{nullptr};
    StoredAlignX *hec2AlxPos{nullptr};
    StoredAlignX *hec1AlxNeg{nullptr};
    StoredAlignX *hec2AlxNeg{nullptr};
    if (detStore()->contains<StoredAlignX> ("HEC1_POS")) {
      if (detStore()->retrieve(hec1AlxPos,"HEC1_POS")!=StatusCode::SUCCESS) {
	ATH_MSG_WARNING(" Unable to retrieve StoredAlignX for the key HEC1_POS");
      }
    }
    if (detStore()->contains<StoredAlignX> ("HEC1_NEG")) {
      if (detStore()->retrieve(hec1AlxNeg,"HEC1_NEG")!=StatusCode::SUCCESS) {
	ATH_MSG_WARNING(" Unable to retrieve StoredAlignX for the key HEC1_NEG");
      }
    }
    if (detStore()->contains<StoredAlignX> ("HEC2_POS")) {
      if (detStore()->retrieve(hec2AlxPos,"HEC2_POS")!=StatusCode::SUCCESS) {
	ATH_MSG_WARNING(" Unable to retrieve StoredAlignX for the key HEC2_POS");
      }
    }
    if (detStore()->contains<StoredAlignX> ("HEC2_NEG")) {
      if (detStore()->retrieve(hec2AlxNeg,"HEC2_NEG")!=StatusCode::SUCCESS) {
	ATH_MSG_WARNING(" Unable to retrieve StoredAlignX for the key HEC2_NEG");
      }
    }
    GeoAlignableTransform *hec1GatPos = hec1AlxPos ? hec1AlxPos->getAlignX(): nullptr;
    GeoAlignableTransform *hec1GatNeg = hec1AlxPos ? hec1AlxNeg->getAlignX(): nullptr;
    GeoAlignableTransform *hec2GatPos = hec2AlxPos ? hec2AlxPos->getAlignX(): nullptr;
    GeoAlignableTransform *hec2GatNeg = hec2AlxPos ? hec2AlxNeg->getAlignX(): nullptr;
    
    // loop over align names
    // if the transform presented alter its delta
    // if the transform is not presented clear its delta
    for(unsigned int i=0; i<alignNames.size(); i++) {
      std::string alignName = alignNames[i];
      HepGeom::Transform3D newDelta;
      // First try to retrieve StoredAlignX
      if(detStore()->contains<StoredAlignX>(alignName)) {
	StoredAlignX* alignX{nullptr};
	sc = detStore()->retrieve(alignX,alignName);

	if(sc.isFailure()) {
	  ATH_MSG_ERROR(" Unable to retrieve StoredAlignX for the key " << alignName);
	  return sc;
	}

	if(!alignX) {
	  ATH_MSG_ERROR(" 0 pointer to StoredAlignX for the key " << alignName);
	  return StatusCode::FAILURE;
	}

	GeoAlignableTransform* gat = alignX->getAlignX();
	if(!gat) {
	  ATH_MSG_ERROR(" 0 pointer to GeoAlignableTransform for the key " << alignName);
	  return StatusCode::FAILURE;
	}

	// check existence of new delta in DetCondKeyTrans
	if(align->getTransform(alignName,newDelta)) {
	  gat->setDelta(Amg::CLHEPTransformToEigen(newDelta));
	}
	else {
	  gat->clearDelta();
	}
      }
      else if (alignName=="HEC_POS") {
	if (hec1GatPos) {
	  if(align->getTransform(alignName,newDelta)) {
	    hec1GatPos->setDelta(Amg::CLHEPTransformToEigen(newDelta));
	  }
	  else {
	    hec1GatPos->clearDelta();
	  }
	}
	if (hec2GatPos) {
	  if(align->getTransform(alignName,newDelta)) {
	    hec2GatPos->setDelta(Amg::CLHEPTransformToEigen(newDelta));
	  }
	  else {
	    hec2GatPos->clearDelta();
	  }
	}
      }
      else if (alignName=="HEC_NEG") {
	if (hec1GatNeg) {
	  if(align->getTransform(alignName,newDelta)) {
	    hec1GatNeg->setDelta(Amg::CLHEPTransformToEigen(newDelta));
	  }
	  else {
	    hec1GatNeg->clearDelta();
	  }
	}
	if (hec2GatNeg) {
	  if(align->getTransform(alignName,newDelta)) {
	    hec2GatNeg->setDelta(Amg::CLHEPTransformToEigen(newDelta));
	  }
	  else {
	    hec2GatNeg->clearDelta();
	  }
	}
      }
      else {
	ATH_MSG_DEBUG(" No StoredAlignX for the key " << alignName);
      }
    } // for
  }
  else {
    ATH_MSG_DEBUG(" No LAr DetCondKeyTrans in SG, skipping align() ");
  }
  
  // debug printout of global positions:
//  for(unsigned int i=0; i<alignNames.size(); i++) 
  for(const std::string& alignName : alignNames) {
    if(detStore()->contains<StoredPhysVol>(alignName)) {
      StoredPhysVol* storedPV{nullptr};
      StatusCode sc = detStore()->retrieve(storedPV,alignName);
      if(sc.isSuccess()) {
	const GeoFullPhysVol* fullPV = storedPV->getPhysVol();
	const GeoTrf::Transform3D& xf =  fullPV->getAbsoluteTransform();
	GeoTrf::Vector3D trans=xf.translation();
	GeoTrf::RotationMatrix3D rot=xf.rotation();
	ATH_MSG_DEBUG("Dump Absolute Transform:");
	ATH_MSG_DEBUG("Key " << alignName << " transl [" << trans.x()
		      << "," << trans.y() << "," << trans.z() << "] rotation \n("
		      << rot(0,0) << "," << rot(0,1) << "," << rot(0,2) << "\n"
		      << rot(1,0) << "," << rot(1,1) << "," << rot(1,2) << "\n"
		      << rot(2,0) << "," << rot(2,1) << "," << rot(2,2) << ")");
      }
    }
  }
  
  return StatusCode::SUCCESS;
}

