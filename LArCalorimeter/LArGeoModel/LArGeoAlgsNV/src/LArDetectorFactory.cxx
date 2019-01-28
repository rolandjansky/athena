/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArDetectorFactory.h"

#include "LArGeoCode/VDetectorParameters.h"
#include "LArGeoRAL/RAL.h"

#include "LArGeoBarrel/BarrelCryostatConstruction.h"
#include "LArGeoTBBarrel/TBBarrelCryostatConstruction.h"
#include "LArGeoEndcap/EndcapCryostatConstruction.h"

#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "CLHEP/Geometry/Transform3D.h" 

#include "LArGeoCode/LArMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelUtilities/GeoDBUtils.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GeoModelUtilities/StoredPhysVol.h"
#include "GeoModelUtilities/StoredAlignX.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "LArReadoutGeometry/HECDetectorManager.h"
#include "LArReadoutGeometry/HECDetDescr.h"
#include "LArReadoutGeometry/HECLongBlock.h"
#include "LArReadoutGeometry/HECDetectorRegion.h"
#include "GeoModelKernel/CellBinning.h"

#include "LArReadoutGeometry/FCALDetectorManager.h"
#include "LArReadoutGeometry/FCALModule.h"
#include "LArReadoutGeometry/FCAL_ChannelMap.h"

#include "LArReadoutGeometry/EMECDetectorManager.h"
#include "LArReadoutGeometry/EMECDetectorRegion.h"
#include "LArReadoutGeometry/EMECDetDescr.h"

#include "LArReadoutGeometry/EMBDetectorManager.h"
#include "LArReadoutGeometry/EMBDetectorRegion.h"
#include "LArReadoutGeometry/EMBDetDescr.h"

LArGeo::LArDetectorFactory::LArDetectorFactory(int testbeam,bool fullGeo)
  : m_detectorManager(),
    m_barrelSagging(false),
    m_barrelVisLimit(-1),
    m_fcalVisLimit(-1),
    m_buildBarrel(true),
    m_buildEndcap(true),
    m_testbeam(testbeam),
    m_fullGeo(fullGeo)
{}


LArGeo::LArDetectorFactory::~LArDetectorFactory()
{
}


// Place the cryostats into a container physical volume.
void LArGeo::LArDetectorFactory::create( GeoPhysVol* a_container )
{


  // Managers:
  EMBDetectorManager   *embDetectorManager=NULL;
  EMECDetectorManager  *emecDetectorManager=NULL;
  HECDetectorManager   *hecDetectorManager=NULL;
  FCALDetectorManager  *fcalDetectorManager=NULL;

  // Tree Tops:
  GeoFullPhysVol *barrelEnvelope=NULL;
  GeoFullPhysVol *endcapEnvelopePos=NULL;
  GeoFullPhysVol *endcapEnvelopeNeg=NULL;

  double projectivityDisplacement(0.);

  if(m_testbeam==0 || m_testbeam==1)
    {
      // 26-Sep-2003 WGS: Get our LAr detector parameters via the NOVA
      // converters.
  
      VDetectorParameters* parameters = 
	(VDetectorParameters *) new  LArGeo::RAL();

      VDetectorParameters::SetInstance(parameters);
    
      // Get access to the material manager:
  
      ISvcLocator *svcLocator = Gaudi::svcLocator();
      IMessageSvc * msgSvc;
      if (svcLocator->service("MessageSvc", msgSvc, true )==StatusCode::FAILURE) {
	throw std::runtime_error("Error in LAr::DetectorFactor, cannot access MessageSvc");
      }

      MsgStream log(msgSvc, "LAr::DetectorFactory"); 

      StoreGateSvc *detStore;
      if (svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
	throw std::runtime_error("Error in LArDetectorFactory, cannot access DetectorStore");
      }

  
      StoredMaterialManager* materialManager = nullptr;
      if (StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) {
	throw std::runtime_error("Error in LArDetectorFactory, cannot access Material Manager");
      }
  
      IRDBAccessSvc* rdbAccess;

      if(svcLocator->service ("RDBAccessSvc",rdbAccess) == StatusCode::FAILURE)
	throw std::runtime_error("Error in BarrelCryostatConstruction, cannot access RDBAccessSvc");
    
      DecodeVersionKey larVersionKey("LAr");
      log << MSG::DEBUG << "Getting primary numbers for " << larVersionKey.node() << ", " << larVersionKey.tag() << endmsg;
    
      IRDBRecordset_ptr larPosition =  rdbAccess->getRecordsetPtr("LArPosition",larVersionKey.tag(),larVersionKey.node());

      if(larPosition->size()==0) 
      {
	larPosition = rdbAccess->getRecordsetPtr("LArPosition", "LArPosition-00");
	if (larPosition->size()==0) 
	  throw std::runtime_error("Error, no lar position table in database!");
      }

      IRDBRecordset_ptr emecGeometry =  rdbAccess->getRecordsetPtr("EmecGeometry",larVersionKey.tag(),larVersionKey.node());
      projectivityDisplacement = (*emecGeometry)[0]->getDouble("ZSHIFT");

      //
      // Patch the materials list by adding special materials.  These
      // cannot be added for the moment to the standard list because they
      // are blended materials and to do the blending requires access to
      // LAr Geometry. 
      //

      LArMaterialManager lArMaterialManager(detStore);
      lArMaterialManager.buildMaterials();
    
      if (m_testbeam==0) {
      
	BarrelCryostatConstruction barrelCryostatConstruction(m_fullGeo);
	barrelCryostatConstruction.setBarrelSagging(m_barrelSagging);
	barrelCryostatConstruction.setBarrelCellVisLimit(m_barrelVisLimit);
            
	EndcapCryostatConstruction endcapCryostatConstruction(m_fullGeo);
	endcapCryostatConstruction.setFCALVisLimit(m_fcalVisLimit);
      
	//const GeoMaterial* matAir = materialManager->getMaterial("std::Air");
      
	if(m_buildBarrel)
	  barrelEnvelope = barrelCryostatConstruction.GetEnvelope();
      
	if(m_buildEndcap)
	  {
	    endcapEnvelopePos = endcapCryostatConstruction.createEnvelope(true);
	    endcapEnvelopeNeg = endcapCryostatConstruction.createEnvelope(false);
	  }

	// Offset of endcaps from nominal position.

	a_container->add(new GeoNameTag("LAr"));
    
	if(m_buildBarrel && m_buildEndcap)
	  {
	    // Typical scenario for each of the Tree Tops:
	    // 1. Construct Alignable Transform from LArPositions
	    // 2. Store FullPV in SG
	    // 3. Store Alignable XF in SG

	    // --- Barrel
	    const IRDBRecord *barrelRec = GeoDBUtils::getTransformRecord(larPosition,"LARCRYO_B"); 
	    if(!barrelRec) throw std::runtime_error("Error, no lar position record in the database");
	    GeoTrf::Transform3D xfBarrel = GeoDBUtils::getTransform(barrelRec);
	    GeoAlignableTransform* barrelAlXf = new GeoAlignableTransform(xfBarrel);

	    {
	      StoredPhysVol *sPhysVol = new StoredPhysVol(barrelEnvelope);
	      StatusCode status=detStore->record(sPhysVol,"LARCRYO_B");
	      if(!status.isSuccess()) throw std::runtime_error("Cannot store LARCRYO_B PhysVol");

	      StoredAlignX *sAlignX = new StoredAlignX(barrelAlXf);
	      status=detStore->record(sAlignX,"LARCRYO_B");
	      if(!status.isSuccess()) throw std::runtime_error("Cannot store LARCRYO_B Alignable XF");
	    }

	    a_container->add(new GeoNameTag("LArBarrel"));
	    a_container->add(barrelAlXf);
	    a_container->add(barrelEnvelope);

	    // --- Endcap Pos
	    const IRDBRecord *posRec = GeoDBUtils::getTransformRecord(larPosition, "LARCRYO_EC_POS");
	    if (!posRec) throw std::runtime_error("Error, no lar position record in the database") ;
	    GeoTrf::Transform3D xfPos = GeoDBUtils::getTransform(posRec);
	    GeoAlignableTransform *xfEndcapPos = new GeoAlignableTransform(xfPos);

	    {
	      StoredPhysVol *sPhysVol = new StoredPhysVol(endcapEnvelopePos);
	      StatusCode status=detStore->record(sPhysVol,"LARCRYO_EC_POS");
	      if(!status.isSuccess()) throw std::runtime_error("Cannot store LARCRYO_EC_POS PhysVol");

	      StoredAlignX *sAlignX = new StoredAlignX(xfEndcapPos);
	      status=detStore->record(sAlignX,"LARCRYO_EC_POS");
	      if(!status.isSuccess()) throw std::runtime_error ("Cannot store LARCRYO_EC_POS");
	    }

	    // --- Endcap Neg
	    const IRDBRecord *negRec = GeoDBUtils::getTransformRecord(larPosition, "LARCRYO_EC_NEG");
	    if (!negRec) throw std::runtime_error("Error, no lar position record in the database") ;
	    GeoTrf::Transform3D xfNeg = GeoDBUtils::getTransform(negRec);
	    GeoAlignableTransform *xfEndcapNeg = new GeoAlignableTransform(xfNeg);

	    {
	      StoredPhysVol *sPhysVol = new StoredPhysVol(endcapEnvelopeNeg);
	      StatusCode status=detStore->record(sPhysVol,"LARCRYO_EC_NEG");
	      if(!status.isSuccess()) throw std::runtime_error("Cannot store LARCRYO_EC_NEG PhysVol");

	      StoredAlignX *sAlignX = new StoredAlignX(xfEndcapNeg);
	      status=detStore->record(sAlignX,"LARCRYO_EC_NEG");
	      if(!status.isSuccess()) throw std::runtime_error ("Cannot store LARCRYO_EC_NEG");
	    }

	    a_container->add( new GeoNameTag("LArEndcapPos"));
	    a_container->add(xfEndcapPos);
	    a_container->add(endcapEnvelopePos);
	    a_container->add( new GeoNameTag("LArEndcapNeg"));
	    a_container->add(xfEndcapNeg);
	    a_container->add( new GeoTransform(GeoTrf::RotateY3D(180.0*Gaudi::Units::deg)));
	    a_container->add(endcapEnvelopeNeg);
	  }
	else if(!m_buildEndcap)
	  {
	    // -- Build the Barrel only
	    const IRDBRecord *barrelRec = GeoDBUtils::getTransformRecord(larPosition,"LARCRYO_B"); 
	    if(!barrelRec) throw std::runtime_error("Error, no lar position record in the database");
	    GeoTrf::Transform3D xfBarrel = GeoDBUtils::getTransform(barrelRec);
	    GeoAlignableTransform* barrelAlXf = new GeoAlignableTransform(xfBarrel);

	    {
	      StoredPhysVol *sPhysVol = new StoredPhysVol(barrelEnvelope);
	      StatusCode status=detStore->record(sPhysVol,"LARCRYO_B");
	      if(!status.isSuccess()) throw std::runtime_error("Cannot store LARCRYO_B PhysVol");

	      StoredAlignX *sAlignX = new StoredAlignX(barrelAlXf);
	      status=detStore->record(sAlignX,"LARCRYO_B");
	      if(!status.isSuccess()) throw std::runtime_error("Cannot store LARCRYO_B Alignable XF");
	    }

	    a_container->add(new GeoNameTag("LArBarrel"));
	    a_container->add(barrelAlXf);
	    a_container->add(barrelEnvelope);

	  }
	else if(!m_buildBarrel)
	  {
	    // --- Endcap Pos
	    const IRDBRecord *posRec = GeoDBUtils::getTransformRecord(larPosition, "LARCRYO_EC_POS");
	    if (!posRec) throw std::runtime_error("Error, no lar position record in the database") ;
	    GeoTrf::Transform3D xfPos = GeoDBUtils::getTransform(posRec);
	    GeoAlignableTransform *xfEndcapPos = new GeoAlignableTransform(xfPos);

	    {
	      StoredPhysVol *sPhysVol = new StoredPhysVol(endcapEnvelopePos);
	      StatusCode status=detStore->record(sPhysVol,"LARCRYO_EC_POS");
	      if(!status.isSuccess()) throw std::runtime_error("Cannot store LARCRYO_EC_POS PhysVol");

	      StoredAlignX *sAlignX = new StoredAlignX(xfEndcapPos);
	      status=detStore->record(sAlignX,"LARCRYO_EC_POS");
	      if(!status.isSuccess()) throw std::runtime_error ("Cannot store LARCRYO_EC_POS");
	    }

	    // --- Endcap Neg
	    const IRDBRecord *negRec = GeoDBUtils::getTransformRecord(larPosition, "LARCRYO_EC_NEG");
	    if (!negRec) throw std::runtime_error("Error, no lar position record in the database") ;
	    GeoTrf::Transform3D xfNeg = GeoDBUtils::getTransform(negRec);
	    GeoAlignableTransform *xfEndcapNeg = new GeoAlignableTransform(xfNeg);

	    {
	      StoredPhysVol *sPhysVol = new StoredPhysVol(endcapEnvelopeNeg);
	      StatusCode status=detStore->record(sPhysVol,"LARCRYO_EC_NEG");
	      if(!status.isSuccess()) throw std::runtime_error("Cannot store LARCRYO_EC_NEG PhysVol");

	      StoredAlignX *sAlignX = new StoredAlignX(xfEndcapNeg);
	      status=detStore->record(sAlignX,"LARCRYO_EC_NEG");
	      if(!status.isSuccess()) throw std::runtime_error ("Cannot store LARCRYO_EC_NEG");
	    }

	    a_container->add( new GeoNameTag("LArEndcapPos"));
	    a_container->add(xfEndcapPos);
	    a_container->add(endcapEnvelopePos);
	    a_container->add( new GeoNameTag("LArEndcapNeg"));
	    a_container->add(xfEndcapNeg);
	    a_container->add( new GeoTransform(GeoTrf::RotateY3D(180.0*Gaudi::Units::deg)));
	    a_container->add(endcapEnvelopeNeg);
      
	  }
      }
      else
	{
	  TBBarrelCryostatConstruction tbbarrelCryostatConstruction;
	  tbbarrelCryostatConstruction.setBarrelSagging(m_barrelSagging);
	  tbbarrelCryostatConstruction.setBarrelCellVisLimit(m_barrelVisLimit);
	
	  barrelEnvelope = tbbarrelCryostatConstruction.GetEnvelope();
	
	  a_container->add(new GeoNameTag("LAr"));
	  a_container->add(barrelEnvelope);
	
	}
    }

  ISvcLocator *svcLocator = Gaudi::svcLocator();
  StoreGateSvc *detStore;
  if (svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in LArDetectorFactory, cannot access DetectorStore");
  }
  IMessageSvc * msgSvc;
  if (svcLocator->service("MessageSvc", msgSvc, true )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in LAr::DetectorFactor, cannot access MessageSvc");
  }
  MsgStream log(msgSvc, "LAr::DetectorFactory"); 
  
  try
  {
    fcalDetectorManager = new FCALDetectorManager();
    std::string fcalModName[2][3]={{"FCAL1_NEG","FCAL2_NEG","FCAL3_NEG"},
				   {"FCAL1_POS","FCAL2_POS","FCAL3_POS"}};
    for (int e=0;e<2;e++) {
      for (int m=0;m<3;m++) {
	StoredPhysVol *vol;
	if(detStore->contains<StoredPhysVol>(fcalModName[e][m])){
	  if (detStore->retrieve(vol,fcalModName[e][m])==StatusCode::SUCCESS) {
	    FCALModule *module=new FCALModule(vol->getPhysVol(),FCALModule::Module(m+1),FCALModule::Endcap(e),projectivityDisplacement);
	    fcalDetectorManager->addModule(module);
	  }
	}
	else
	  log << MSG::DEBUG << " No Stored PV for " << fcalModName[e][m] << " in Detector Store" << endmsg;
      }
    }
  }
  catch (std::exception & ex) {
   log << MSG::WARNING << "Unable to build FCAL detector manager. " << ex.what() << endmsg;
  }

  try
  {
    hecDetectorManager = new  HECDetectorManager();
    for (unsigned int s=0;s<4;s++) {
      for (unsigned int r=0;r<2;r++) {
	unsigned int nPhi    = r==0? 2:1;
       	for (unsigned int endcap=0;endcap<2;endcap++) {
	  StoredPhysVol *vol;               
	  std::string nameTag= endcap==0 ? (s<2 ? "HEC1_NEG":"HEC2_NEG") : (s<2 ? "HEC1_POS":"HEC2_POS");
	  if(detStore->contains<StoredPhysVol>(nameTag)){
	    if (StatusCode::SUCCESS==detStore->retrieve(vol, nameTag)) {                
	      
	      
	      unsigned int width = 32;
	      double startPhi =  endcap==0?  M_PI : 0;
	      double endPhi   =  endcap==0? -M_PI : 2*M_PI ;
	      
	      CellBinning  phiBinning(startPhi, endPhi, width*nPhi);
	      HECDetDescr *d = new HECDetDescr(hecDetectorManager,s,r,phiBinning); 
	      
	      HECDetectorRegion::DetectorSide side = (HECDetectorRegion::DetectorSide) endcap;
	      HECDetectorRegion *region = new HECDetectorRegion(vol->getPhysVol(),d,side,projectivityDisplacement);
	      hecDetectorManager->addDetectorRegion(region);
	    }         
	  }
	  else
	    log << MSG::DEBUG << " No Stored PV for " << nameTag
		<< " in Detector Store" << endmsg;                                                 
	}
      }
    }
  }
  catch (std::exception & ex) {
    log << MSG::WARNING << "Unable to build HEC detector manager. " << ex.what() << endmsg;
  }
  
  
  try
  {
    
    
    emecDetectorManager  = new EMECDetectorManager();
    
    // Here is a table of min and max eta for different sampling layers, radial part (i/o) and region.
    
    
    
    for (int e=0;e<2;e++) {
      
      double startPhi =  e==0?  M_PI-2*M_PI/768/2 : -2*M_PI/768/2;
      double endPhi   =  e==0? -M_PI-2*M_PI/768/2 : 2*M_PI-2*M_PI/768/2 ;


      StoredPhysVol *sPhys;
      
      if(detStore->contains<StoredPhysVol>(e==0 ? "EMEC_OUTER_WHEEL_NEG" : "EMEC_OUTER_WHEEL_POS")){
	if (detStore->retrieve(sPhys,e==0 ? "EMEC_OUTER_WHEEL_NEG" : "EMEC_OUTER_WHEEL_POS")==StatusCode::SUCCESS) 
	{
	  GeoFullPhysVol *emecEnvelope=(GeoFullPhysVol *) sPhys->getPhysVol();
	  
	  
	  {
	    CellBinning phiBinning(startPhi,endPhi,64);
	    EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,1,0,0,phiBinning);
	    EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::DetectorSide(e),projectivityDisplacement);
	    emecDetectorManager->addDetectorRegion(region);
	  }
	  
	  // Outer Wheel Sampling 1 Region 1:
	  {
	    CellBinning phiBinning(startPhi,endPhi,64);
	    EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,1,1,0,phiBinning);
	    EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::DetectorSide(e),projectivityDisplacement);
	    emecDetectorManager->addDetectorRegion(region);
	  }
	  // Outer Wheel Sampling 1 Region 2:
	  {
	    CellBinning phiBinning(startPhi,endPhi,64);
	    EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,1,2,0,phiBinning);
	    EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::DetectorSide(e),projectivityDisplacement);
	    emecDetectorManager->addDetectorRegion(region);
	  }
	  // Outer Wheel Sampling 1 Region 3:
	  {
	    CellBinning phiBinning(startPhi,endPhi,64);
	    EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,1,3,0,phiBinning);
	    EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::DetectorSide(e),projectivityDisplacement);
	    emecDetectorManager->addDetectorRegion(region);
	  }
	  // Outer Wheel Sampling 1 Region 4:
	  {
	    CellBinning phiBinning(startPhi,endPhi,64);
	    EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,1,4,0,phiBinning);
	    EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::DetectorSide(e),projectivityDisplacement);
	    emecDetectorManager->addDetectorRegion(region);
	  }
	  // Outer Wheel Sampling 1 Region 5:
	  {
	    CellBinning phiBinning(startPhi,endPhi,64);
	    EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,1,5,0,phiBinning);
	    EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::DetectorSide(e),projectivityDisplacement);
	    emecDetectorManager->addDetectorRegion(region);
	  }
	  // Outer Wheel Sampling 2 Region 0:
	  {
	    CellBinning phiBinning(startPhi,endPhi,256);
	    EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,2,0,0,phiBinning);
	    EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::DetectorSide(e),projectivityDisplacement);
	    emecDetectorManager->addDetectorRegion(region);
	  }
	  // Outer Wheel Sampling 2 Region 1:
	  {
	    CellBinning phiBinning(startPhi,endPhi,256);
	    EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,2,1,0,phiBinning);
	    EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::DetectorSide(e),projectivityDisplacement);
	    emecDetectorManager->addDetectorRegion(region);
	  }
	  // Outer Wheel Sampling 3 Region 0:
	  {
	    CellBinning phiBinning(startPhi,endPhi,256);
	    EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,3,0,0,phiBinning);
	    EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::DetectorSide(e),projectivityDisplacement);
	    emecDetectorManager->addDetectorRegion(region);
	  }
	}
      }
      else
	log << MSG::DEBUG << " No Stored PV for " << (e==0 ? "EMEC_OUTER_WHEEL_NEG" : "EMEC_OUTER_WHEEL_POS")
	    << " in Detector Store" << endmsg;

      if(detStore->contains<StoredPhysVol>(e==0 ? "EMEC_INNER_WHEEL_NEG" : "EMEC_INNER_WHEEL_POS")){
	if (detStore->retrieve(sPhys,e==0 ? "EMEC_INNER_WHEEL_NEG" : "EMEC_INNER_WHEEL_POS")==StatusCode::SUCCESS) 
	{
	  GeoFullPhysVol *emecEnvelope=(GeoFullPhysVol *) sPhys->getPhysVol();
	  
	  // Inner Wheel Sampling 1 Region 0:
	  {
	    CellBinning phiBinning(startPhi,endPhi,64);
	    EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,1,0,1,phiBinning);
	    EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::DetectorSide(e),projectivityDisplacement);
	    emecDetectorManager->addDetectorRegion(region);
	  }
	  // Inner Wheel Sampling 2 Region 0:
	  
	  {
	    CellBinning phiBinning(startPhi,endPhi,64);
	    EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,2,0,1,phiBinning);
	    EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::DetectorSide(e),projectivityDisplacement);
	    emecDetectorManager->addDetectorRegion(region);
	  }
	  
	}
      }
      else
      	log << MSG::DEBUG << " No Stored PV for " << (e==0 ? "EMEC_INNER_WHEEL_NEG" : "EMEC_INNER_WHEEL_POS")
	    << " in Detector Store" << endmsg;
      
      StoredPhysVol *sPresamplerEnvelope; 

      if(detStore->contains<StoredPhysVol>(e==0 ? "PRESAMPLER_EC_NEG":"PRESAMPLER_EC_POS" )){
	if (StatusCode::SUCCESS==detStore->retrieve(sPresamplerEnvelope, e==0 ? "PRESAMPLER_EC_NEG":"PRESAMPLER_EC_POS" )) {        
	  GeoFullPhysVol * PresamplerEnvelope=sPresamplerEnvelope->getPhysVol();
	  CellBinning presamplerPhiBinning(startPhi,endPhi,64);
	  EMECDetDescr *presamplerDetDescr = new EMECDetDescr(emecDetectorManager,0,0,0,presamplerPhiBinning);
	  EMECDetectorRegion *presamplerRegion = new EMECDetectorRegion(PresamplerEnvelope,presamplerDetDescr,EMECDetectorRegion::DetectorSide(e),projectivityDisplacement);
	  emecDetectorManager->addDetectorRegion(presamplerRegion);
	}                                                            
      }
      else      
	log << MSG::DEBUG << " No Stored PV for " << (e==0 ? "PRESAMPLER_EC_NEG":"PRESAMPLER_EC_POS")
	    << " in Detector Store" << endmsg;
    }
    
  }
  catch (std::exception & ex) {
    log << MSG::WARNING << "Unable to build EMEC detector manager. " << ex.what() << endmsg;
  }

  try
  { 
    embDetectorManager  = new EMBDetectorManager();
    int firstEndcap=m_testbeam==0 ? 0:1, endEndcap=2;
    for (int e= firstEndcap ;e<endEndcap;e++) {

      double startPhi =  e==0?  M_PI-2*M_PI/1024/2 : -2*M_PI/1024/2;
      double endPhi   =  e==0? -M_PI-2*M_PI/1024/2 : 2*M_PI-2*M_PI/1024/2 ;
      int    NDIV     = 1;
      if (m_testbeam==1) {
	startPhi=0*M_PI/180;
	endPhi  =22.5*M_PI/180;
	NDIV=16;
      }
      
      // Sampling layer 3 region 0:
      if (1)
	{
	  StoredPhysVol *sPhys;
	  if(detStore->contains<StoredPhysVol>(e==0 ? "EMB_NEG" : "EMB_POS"))
	  {
	    if (detStore->retrieve(sPhys,e==0 ? "EMB_NEG" : "EMB_POS")==StatusCode::SUCCESS) 
	    {
	      CellBinning phiBinning(startPhi,endPhi,256/NDIV);
	      EMBDetDescr *detDescr = new EMBDetDescr(embDetectorManager,3,0,phiBinning);
	      GeoFullPhysVol *embEnvelope=(GeoFullPhysVol *) sPhys->getPhysVol();
	      EMBDetectorRegion *region = new EMBDetectorRegion(embEnvelope,detDescr,EMBDetectorRegion::DetectorSide(e));
	      embDetectorManager->addDetectorRegion(region);
	    }
	  }
	  else
	    log << MSG::DEBUG << " No Stored PV for " << (e==0 ? "EMB_NEG" : "EMB_POS" )<< " in Detector Store" << endmsg;
	}
      // Sampling layer 2 region 0:
      if (1)
	{
	  
	  StoredPhysVol *sPhys;
	  if(detStore->contains<StoredPhysVol>(e==0 ? "EMB_NEG" : "EMB_POS"))
	  {
	    if (detStore->retrieve(sPhys,e==0 ? "EMB_NEG" : "EMB_POS")==StatusCode::SUCCESS) 
	    {
	      CellBinning phiBinning(startPhi,endPhi,256/NDIV);
	      EMBDetDescr *detDescr = new EMBDetDescr(embDetectorManager,2,0,phiBinning);
	      GeoFullPhysVol *embEnvelope=(GeoFullPhysVol *) sPhys->getPhysVol();
	      EMBDetectorRegion *region = new EMBDetectorRegion(embEnvelope,detDescr,EMBDetectorRegion::DetectorSide(e));
	      embDetectorManager->addDetectorRegion(region);
	    }
	  }
	  else
	    log << MSG::DEBUG << " No Stored PV for " << (e==0 ? "EMB_NEG" : "EMB_POS" )<< " in Detector Store" << endmsg;
	}
      // Sampling layer 2 region 1:
      if (1)
	{
	  StoredPhysVol *sPhys;
	  if(detStore->contains<StoredPhysVol>(e==0 ? "EMB_NEG" : "EMB_POS"))
	  {
	    if (detStore->retrieve(sPhys,e==0 ? "EMB_NEG" : "EMB_POS")==StatusCode::SUCCESS) 
	    {
	      CellBinning phiBinning(startPhi,endPhi,256/NDIV);
	      EMBDetDescr *detDescr = new EMBDetDescr(embDetectorManager,2,1,phiBinning);
	      GeoFullPhysVol *embEnvelope=(GeoFullPhysVol *) sPhys->getPhysVol();
	      EMBDetectorRegion *region = new EMBDetectorRegion(embEnvelope,detDescr,EMBDetectorRegion::DetectorSide(e));
	      embDetectorManager->addDetectorRegion(region);
	    }
	  }
	  else
	    log << MSG::DEBUG << " No Stored PV for " << (e==0 ? "EMB_NEG" : "EMB_POS") << " in Detector Store" << endmsg;
	}
      // Sampling layer 1 region 0:
      if (1)
	{
	  StoredPhysVol *sPhys;
	  if(detStore->contains<StoredPhysVol>(e==0 ? "EMB_NEG" : "EMB_POS"))
	  {
	    if (detStore->retrieve(sPhys,e==0 ? "EMB_NEG" : "EMB_POS")==StatusCode::SUCCESS) 
	    {
	      CellBinning phiBinning(startPhi,endPhi,64/NDIV);
	      EMBDetDescr *detDescr = new EMBDetDescr(embDetectorManager,1,0,phiBinning);
	      GeoFullPhysVol *embEnvelope=(GeoFullPhysVol *) sPhys->getPhysVol();
	      EMBDetectorRegion *region = new EMBDetectorRegion(embEnvelope,detDescr,EMBDetectorRegion::DetectorSide(e));
	      embDetectorManager->addDetectorRegion(region);
	    }
	  }
	  else
	    log << MSG::DEBUG << " No Stored PV for " << (e==0 ? "EMB_NEG" : "EMB_POS") << " in Detector Store" << endmsg;
	}
      // Sampling layer 1 region 0:
      if (1)
	{
	  StoredPhysVol *sPhys;
	  if(detStore->contains<StoredPhysVol>(e==0 ? "EMB_NEG" : "EMB_POS"))
	  {
	    if (detStore->retrieve(sPhys,e==0 ? "EMB_NEG" : "EMB_POS")==StatusCode::SUCCESS) 
	    {
	      CellBinning phiBinning(startPhi,endPhi,256/NDIV);
	      EMBDetDescr *detDescr = new EMBDetDescr(embDetectorManager,1,1,phiBinning);
	      GeoFullPhysVol *embEnvelope=(GeoFullPhysVol *) sPhys->getPhysVol();
	      EMBDetectorRegion *region = new EMBDetectorRegion(embEnvelope,detDescr,EMBDetectorRegion::DetectorSide(e));
	      embDetectorManager->addDetectorRegion(region);
	    }
	  }
	  else
	    log << MSG::DEBUG << " No Stored PV for " << (e==0 ? "EMB_NEG" : "EMB_POS") << " in Detector Store" << endmsg;
	}
      // Sampling layer 0 region 0:
      if (1)
	{
	  StoredPhysVol *sPhys;
	  if(detStore->contains<StoredPhysVol>(e==0 ? "EMB_NEG" : "EMB_POS"))
	  {
	    if (detStore->retrieve(sPhys,e==0 ? "EMB_NEG" : "EMB_POS")==StatusCode::SUCCESS) 
	    {
	      CellBinning phiBinning(startPhi,endPhi,64/NDIV);
	      EMBDetDescr *detDescr = new EMBDetDescr(embDetectorManager,0,0,phiBinning);
	      GeoFullPhysVol *embEnvelope=(GeoFullPhysVol *) sPhys->getPhysVol();
	      EMBDetectorRegion *region = new EMBDetectorRegion(embEnvelope,detDescr,EMBDetectorRegion::DetectorSide(e));
	      embDetectorManager->addDetectorRegion(region);
	    }
	  }
	  else
	    log << MSG::DEBUG << " No Stored PV for " << (e==0 ? "EMB_NEG" : "EMB_POS") << " in Detector Store" << endmsg;
	}
    }
  }
  catch (std::exception & ex) {
    log << MSG::WARNING << "Unable to build EMB detector manager. " << ex.what() << endmsg;
  }

  m_detectorManager = new LArDetectorManager(embDetectorManager,emecDetectorManager,hecDetectorManager,fcalDetectorManager);
  m_detectorManager->isTestBeam(false);
  if (barrelEnvelope)    m_detectorManager->addTreeTop(barrelEnvelope);
  if (endcapEnvelopePos) m_detectorManager->addTreeTop(endcapEnvelopePos);
  if (endcapEnvelopeNeg) m_detectorManager->addTreeTop(endcapEnvelopeNeg);
  
  StatusCode sc;
  if (embDetectorManager)  
  {
    sc = detStore->record(embDetectorManager,  embDetectorManager->getName());
    if(sc.isFailure())
      log << MSG::ERROR << "Unable to record embDetectorManager" << endmsg;
  }

  if (emecDetectorManager) 
  {
    sc = detStore->record(emecDetectorManager, emecDetectorManager->getName());
    if(sc.isFailure())
      log << MSG::ERROR << "Unable to record emecDetectorManager" << endmsg;
  }

  if (hecDetectorManager)    
  {
    sc = detStore->record(hecDetectorManager,  hecDetectorManager->getName());
    if(sc.isFailure())
      log << MSG::ERROR << "Unable to record hecDetectorManager" << endmsg;
  }

  if (fcalDetectorManager)     
  {
    sc = detStore->record(fcalDetectorManager, fcalDetectorManager->getName());
    if(sc.isFailure())
      log << MSG::ERROR << "Unable to record fcalDetectorManager" << endmsg;
  }
}



const LArDetectorManager* LArGeo::LArDetectorFactory::getDetectorManager() const
{
  return m_detectorManager;
}

