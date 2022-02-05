/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArDetectorFactory.h"
#include "LArReadoutGeometryBuilder.h"

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

LArGeo::LArDetectorFactory::LArDetectorFactory(StoreGateSvc* detStore
					       , const LArHVManager* hvManager
					       , int testbeam
					       , bool fullGeo)
  : m_detectorManager(nullptr)
  , m_detStore(detStore)
  , m_hvManager(hvManager)
  , m_barrelSagging(false)
  , m_barrelVisLimit(-1)
  , m_fcalVisLimit(-1)
  , m_buildBarrel(true)
  , m_buildEndcap(true)
  , m_testbeam(testbeam)
  , m_fullGeo(fullGeo)
  , m_activateFT(false)
  , m_enableMBTS(true)
{}


LArGeo::LArDetectorFactory::~LArDetectorFactory()
{
}


// Place the cryostats into a container physical volume.
void LArGeo::LArDetectorFactory::create( GeoPhysVol* a_container )
{
  // Tree Tops:
  GeoFullPhysVol *barrelEnvelope=nullptr;
  GeoFullPhysVol *endcapEnvelopePos=nullptr;
  GeoFullPhysVol *endcapEnvelopeNeg=nullptr;

  double projectivityDisplacement(0.);

  if(m_testbeam==0 || m_testbeam==1) {
      m_parameters = std::make_unique<LArGeo::RAL>();

      // Get access to the material manager:

      ISvcLocator *svcLocator = Gaudi::svcLocator();
      IMessageSvc * msgSvc;
      if (svcLocator->service("MessageSvc", msgSvc, true )==StatusCode::FAILURE) {
	throw std::runtime_error("Error in LAr::DetectorFactor, cannot access MessageSvc");
      }

      MsgStream log(msgSvc, "LAr::DetectorFactory");

      StoredMaterialManager* materialManager = nullptr;
      if (StatusCode::SUCCESS != m_detStore->retrieve(materialManager, std::string("MATERIALS"))) {
	throw std::runtime_error("Error in LArDetectorFactory, cannot access Material Manager");
      }

      IRDBAccessSvc* rdbAccess = nullptr;

      if(svcLocator->service ("RDBAccessSvc",rdbAccess) == StatusCode::FAILURE)
	throw std::runtime_error("Error in BarrelCryostatConstruction, cannot access RDBAccessSvc");

      DecodeVersionKey larVersionKey("LAr");
      log << MSG::DEBUG << "Getting primary numbers for " << larVersionKey.node() << ", " << larVersionKey.tag() << endmsg;

      IRDBRecordset_ptr larPosition =  rdbAccess->getRecordsetPtr("LArPosition",larVersionKey.tag(),larVersionKey.node());

      if(larPosition->size()==0) {
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

      LArMaterialManager lArMaterialManager(m_detStore);
      lArMaterialManager.buildMaterials();

      if (m_testbeam==0) {

	BarrelCryostatConstruction barrelCryostatConstruction(m_fullGeo, m_activateFT);
	barrelCryostatConstruction.setBarrelSagging(m_barrelSagging);
	barrelCryostatConstruction.setBarrelCellVisLimit(m_barrelVisLimit);

	EndcapCryostatConstruction endcapCryostatConstruction(m_fullGeo, m_EMECVariantInner, m_EMECVariantOuter, m_activateFT, m_enableMBTS);
	endcapCryostatConstruction.setFCALVisLimit(m_fcalVisLimit);

	if(m_buildBarrel) {
	  barrelEnvelope = barrelCryostatConstruction.GetEnvelope(m_parameters.get());
	}

	if(m_buildEndcap) {
	  endcapEnvelopePos = endcapCryostatConstruction.createEnvelope(true);
	  endcapEnvelopeNeg = endcapCryostatConstruction.createEnvelope(false);
	}

	// Offset of endcaps from nominal position.
	a_container->add(new GeoNameTag("LAr"));

	if(m_buildBarrel && m_buildEndcap) {

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
	    StatusCode status=m_detStore->record(sPhysVol,"LARCRYO_B");
	    if(!status.isSuccess()) throw std::runtime_error("Cannot store LARCRYO_B PhysVol");

	    StoredAlignX *sAlignX = new StoredAlignX(barrelAlXf);
	    status=m_detStore->record(sAlignX,"LARCRYO_B");
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
	    StatusCode status=m_detStore->record(sPhysVol,"LARCRYO_EC_POS");
	    if(!status.isSuccess()) throw std::runtime_error("Cannot store LARCRYO_EC_POS PhysVol");
	    
	    StoredAlignX *sAlignX = new StoredAlignX(xfEndcapPos);
	    status=m_detStore->record(sAlignX,"LARCRYO_EC_POS");
	    if(!status.isSuccess()) throw std::runtime_error ("Cannot store LARCRYO_EC_POS");
	  }
	  
	  // --- Endcap Neg
	  const IRDBRecord *negRec = GeoDBUtils::getTransformRecord(larPosition, "LARCRYO_EC_NEG");
	  if (!negRec) throw std::runtime_error("Error, no lar position record in the database") ;
	  GeoTrf::Transform3D xfNeg = GeoDBUtils::getTransform(negRec);
	  GeoAlignableTransform *xfEndcapNeg = new GeoAlignableTransform(xfNeg);

	  {
	    StoredPhysVol *sPhysVol = new StoredPhysVol(endcapEnvelopeNeg);
	    StatusCode status=m_detStore->record(sPhysVol,"LARCRYO_EC_NEG");
	    if(!status.isSuccess()) throw std::runtime_error("Cannot store LARCRYO_EC_NEG PhysVol");
	    
	    StoredAlignX *sAlignX = new StoredAlignX(xfEndcapNeg);
	    status=m_detStore->record(sAlignX,"LARCRYO_EC_NEG");
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
	else if(!m_buildEndcap) {
	  // -- Build the Barrel only
	  const IRDBRecord *barrelRec = GeoDBUtils::getTransformRecord(larPosition,"LARCRYO_B");
	  if(!barrelRec) throw std::runtime_error("Error, no lar position record in the database");
	  GeoTrf::Transform3D xfBarrel = GeoDBUtils::getTransform(barrelRec);
	  GeoAlignableTransform* barrelAlXf = new GeoAlignableTransform(xfBarrel);
	  
	  {
	    StoredPhysVol *sPhysVol = new StoredPhysVol(barrelEnvelope);
	    StatusCode status=m_detStore->record(sPhysVol,"LARCRYO_B");
	    if(!status.isSuccess()) throw std::runtime_error("Cannot store LARCRYO_B PhysVol");

	    StoredAlignX *sAlignX = new StoredAlignX(barrelAlXf);
	    status=m_detStore->record(sAlignX,"LARCRYO_B");
	    if(!status.isSuccess()) throw std::runtime_error("Cannot store LARCRYO_B Alignable XF");
	  }

	  a_container->add(new GeoNameTag("LArBarrel"));
	  a_container->add(barrelAlXf);
	  a_container->add(barrelEnvelope);
	  
	}
	else if(!m_buildBarrel) {
	  // --- Endcap Pos
	  const IRDBRecord *posRec = GeoDBUtils::getTransformRecord(larPosition, "LARCRYO_EC_POS");
	  if (!posRec) throw std::runtime_error("Error, no lar position record in the database") ;
	  GeoTrf::Transform3D xfPos = GeoDBUtils::getTransform(posRec);
	  GeoAlignableTransform *xfEndcapPos = new GeoAlignableTransform(xfPos);
	  
	  {
	    StoredPhysVol *sPhysVol = new StoredPhysVol(endcapEnvelopePos);
	    StatusCode status=m_detStore->record(sPhysVol,"LARCRYO_EC_POS");
	    if(!status.isSuccess()) throw std::runtime_error("Cannot store LARCRYO_EC_POS PhysVol");
	    
	    StoredAlignX *sAlignX = new StoredAlignX(xfEndcapPos);
	    status=m_detStore->record(sAlignX,"LARCRYO_EC_POS");
	    if(!status.isSuccess()) throw std::runtime_error ("Cannot store LARCRYO_EC_POS");
	  }

	  // --- Endcap Neg
	  const IRDBRecord *negRec = GeoDBUtils::getTransformRecord(larPosition, "LARCRYO_EC_NEG");
	  if (!negRec) throw std::runtime_error("Error, no lar position record in the database") ;
	  GeoTrf::Transform3D xfNeg = GeoDBUtils::getTransform(negRec);
	  GeoAlignableTransform *xfEndcapNeg = new GeoAlignableTransform(xfNeg);

	  {
	    StoredPhysVol *sPhysVol = new StoredPhysVol(endcapEnvelopeNeg);
	    StatusCode status=m_detStore->record(sPhysVol,"LARCRYO_EC_NEG");
	    if(!status.isSuccess()) throw std::runtime_error("Cannot store LARCRYO_EC_NEG PhysVol");
	    
	    StoredAlignX *sAlignX = new StoredAlignX(xfEndcapNeg);
	    status=m_detStore->record(sAlignX,"LARCRYO_EC_NEG");
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
      else {
	TBBarrelCryostatConstruction tbbarrelCryostatConstruction;
	tbbarrelCryostatConstruction.setBarrelSagging(m_barrelSagging);
	tbbarrelCryostatConstruction.setBarrelCellVisLimit(m_barrelVisLimit);
	
	barrelEnvelope = tbbarrelCryostatConstruction.GetEnvelope(m_parameters.get());
	
	a_container->add(new GeoNameTag("LAr"));
	a_container->add(barrelEnvelope);
	
      }
  }

  auto subDetManagers = buildLArReadoutGeometry(m_detStore
						, m_hvManager
						, Athena::getMessageSvc()
						, m_testbeam
						, projectivityDisplacement);
  
  EMBDetectorManager*  embDetectorManager{std::get<0>(subDetManagers)};
  EMECDetectorManager* emecDetectorManager{std::get<1>(subDetManagers)};
  HECDetectorManager*  hecDetectorManager{std::get<2>(subDetManagers)};
  FCALDetectorManager* fcalDetectorManager{std::get<3>(subDetManagers)};
  
  if(!embDetectorManager
     || !emecDetectorManager
     || !hecDetectorManager
     || !fcalDetectorManager) {
    std::string errorMessage="Failed to build LAr Readout Geometry description";
    throw std::runtime_error(errorMessage);
  }

  m_detectorManager = new LArDetectorManager(embDetectorManager,emecDetectorManager,hecDetectorManager,fcalDetectorManager);
  m_detectorManager->isTestBeam(false);
  if (barrelEnvelope)    m_detectorManager->addTreeTop(barrelEnvelope);
  if (endcapEnvelopePos) m_detectorManager->addTreeTop(endcapEnvelopePos);
  if (endcapEnvelopeNeg) m_detectorManager->addTreeTop(endcapEnvelopeNeg);
}



const LArDetectorManager* LArGeo::LArDetectorFactory::getDetectorManager() const
{
  return m_detectorManager;
}

std::unique_ptr<LArGeo::VDetectorParameters> LArGeo::LArDetectorFactory::moveParameters()
{
  return std::move (m_parameters);
}

