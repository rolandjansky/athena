/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GeometryJiveXML/GeometryWriter.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"

#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "TileDetDescr/TileDetDescrManager.h"

#include "LArReadoutGeometry/LArDetectorManager.h"
#include "GeoModelKernel/GeoTrd.h"

#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "TRT_ReadoutGeometry/TRT_DetectorManager.h"
#include "TRT_ReadoutGeometry/TRT_BaseElement.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"


#include <fstream>

namespace JiveXML {

  StatusCode GeometryWriter::initialize()
  {
    ATH_CHECK(m_caloMgrKey.initialize());
    return StatusCode::SUCCESS;
  }

  StatusCode GeometryWriter::writeGeometry() 
  {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "writeGeometry()" << endmsg;

    std::ofstream outputFile("AGeometry.xml");
    writeHeader(outputFile);

    if ( detStore()->retrieve(m_pixel_manager, "Pixel").isFailure() ){
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not retrieve InDetDD::PixelDetectorManager" << endmsg;
    } else {
      if (detStore()->retrieve(m_pixelIdHelper, "PixelID").isFailure() ){
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not retrieve PixelIDHelper" << endmsg;
      } else {
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved PixelIDHelper" << endmsg;
	writePixelGeometry(outputFile);
      }
    }

    if ( detStore()->retrieve(m_silicon_manager, "SCT").isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not retrieve InDetDD::SCT_DetectorManager" << endmsg;
    } else {
      if (detStore()->retrieve(m_sctIdHelper, "SCT_ID").isFailure() ){
        if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not retrieve SCT_IDHelper" << endmsg;
      } else {
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved SCT_IDHelper" << endmsg;
	writeSCTGeometry(outputFile);
      }
    }

    if ( detStore()->retrieve(m_trt_manager, "TRT").isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not retrieve InDetDD::TRT_DetectorManager" << endmsg;
    } else {
// No SGKey ? As in graphics/VP1/VP1Systems/VP12DGeometrySystems/StoreGateGeometryReader
//      if (detStore()->retrieve(m_trtIdHelper, "TRT_ID").isFailure() ){
      if (detStore()->retrieve(m_trtIdHelper, "").isFailure() ){
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not retrieve TRT_IDHelper" << endmsg;
      } else {
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved TRT_IDHelper" << endmsg;
	writeTRTGeometry(outputFile);
      }
    }

    writeSolenoidGeometry(outputFile);

    SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey};
    if(!caloMgrHandle.isValid()) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not retrieve CaloDetDescrManager" << endmsg;
    } else {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved CaloDetDescrManager" << endmsg;
      // Hardcoded LAr zMin, zMax, rMin, rMax
      m_larMin.reserve(m_numCaloTypes);
      m_larMax.reserve(m_numCaloTypes);
      m_larMin[CaloCell_ID::PreSamplerB] = 0.0;
      m_larMax[CaloCell_ID::PreSamplerB] = 340.0;
      m_larMin[CaloCell_ID::EMB1] = 0.0;
      m_larMax[CaloCell_ID::EMB1] = 340.0;
      m_larMin[CaloCell_ID::EMB2] = 0.0;
      m_larMax[CaloCell_ID::EMB2] = 340.0;
      m_larMin[CaloCell_ID::EMB3] = 0.0;
      m_larMax[CaloCell_ID::EMB3] = 340.0;
      m_larMin[CaloCell_ID::PreSamplerE] = 29.0;
      m_larMax[CaloCell_ID::PreSamplerE] = 203.4;
      m_larMin[CaloCell_ID::EME1] = 29.0;
      m_larMax[CaloCell_ID::EME1] = 203.4;
      m_larMin[CaloCell_ID::EME2] = 29.0;
      m_larMax[CaloCell_ID::EME2] = 203.4;
      m_larMin[CaloCell_ID::EME3] = 29.0;
      m_larMax[CaloCell_ID::EME3] = 203.4;
      m_larMin[CaloCell_ID::HEC0] = 37.2;
      m_larMax[CaloCell_ID::HEC0] = 203.4;
      m_larMin[CaloCell_ID::HEC1] = 47.5;
      m_larMax[CaloCell_ID::HEC1] = 203.4;
      m_larMin[CaloCell_ID::HEC2] = 47.5;
      m_larMax[CaloCell_ID::HEC2] = 203.4;
      m_larMin[CaloCell_ID::HEC3] = 47.5;
      m_larMax[CaloCell_ID::HEC3] = 203.4;
      writeLArGeometry(outputFile,*caloMgrHandle);
    }

    if ( detStore()->retrieve(m_tile_manager, "Tile").isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not retrieve TileDetDescrManager" << endmsg;
    } else {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved TileDetDescrManager" << endmsg;
      writeTILEGeometry(outputFile);
    }
    
    if ( detStore()->retrieve(m_lar_manager, "LArMgr").isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not retrieve LArDetectorManager for MBTS" << endmsg;
    } else {	
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved LArDetectorManager for MBTS" << endmsg;
      writeMBTSGeometry(outputFile);
    }
    writeFooter(outputFile);
    outputFile.close();

    return StatusCode::SUCCESS;
  }

  void GeometryWriter::writeHeader(std::ofstream &out)
  {
    out << "<?xml version=\"1.0\"?>" << std::endl
	<< "<!DOCTYPE AGeometry [" << std::endl
	<< "<!ELEMENT AGeometry (ADisc | ARectangle | ABarrelCalorimeter |" << std::endl 
	<< "                     AEndcapCalorimeter | AGapCalorimeter | AEndcapCryostat |" << std::endl
	<< "                     ABarrelSiliconDetector | AEndcapSiliconDetector |" << std::endl
	<< "                     ABarrelTRTDetector | AEndcapTRTDetector)* >" << std::endl
	<< "<!ELEMENT ADisc EMPTY >" << std::endl
	<< "<!ATTLIST ADisc" << std::endl
	<< "          p          CDATA     #REQUIRED" << std::endl
	<< "          c          CDATA     #REQUIRED" << std::endl
	<< "          n          CDATA     #REQUIRED" << std::endl
	<< "          rIn        CDATA     #REQUIRED" << std::endl
	<< "          rOut       CDATA     #REQUIRED" << std::endl
	<< "          nIn        CDATA     \"256\"" << std::endl
	<< "          nOut       CDATA     \"256\">" << std::endl
	<< "<!ELEMENT ARectangle EMPTY >" << std::endl
	<< "<!ATTLIST ARectangle" << std::endl
	<< "          p          CDATA     #REQUIRED" << std::endl
	<< "          c          CDATA     #REQUIRED" << std::endl
	<< "          n          CDATA     #REQUIRED" << std::endl
	<< "          xMin       CDATA     #REQUIRED" << std::endl
	<< "          xMax       CDATA     #REQUIRED" << std::endl
	<< "          yMin       CDATA     #REQUIRED" << std::endl
	<< "          yMax       CDATA     #REQUIRED" << std::endl
	<< "          xR         (YES|NO)  \"YES\"" << std::endl
	<< "          yR         (YES|NO)  \"YES\">" << std::endl
	<< "<!ELEMENT ABarrelCalorimeter EMPTY >" << std::endl
	<< "<!ATTLIST ABarrelCalorimeter" << std::endl
	<< "          c          CDATA     #REQUIRED" << std::endl
	<< "          n          CDATA     #REQUIRED" << std::endl
	<< "          sampling   CDATA     #REQUIRED" << std::endl
	<< "          region     CDATA     #REQUIRED" << std::endl
	<< "          rMin       CDATA     #REQUIRED" << std::endl
	<< "          rMax       CDATA     #REQUIRED" << std::endl
	<< "          zMin       CDATA     #REQUIRED" << std::endl
	<< "          zMax       CDATA     #REQUIRED" << std::endl
	<< "          eta0       CDATA     #REQUIRED" << std::endl
	<< "          deta       CDATA     #REQUIRED" << std::endl
	<< "          neta       CDATA     #REQUIRED" << std::endl
	<< "          meta       CDATA     #REQUIRED" << std::endl
	<< "          phi0       CDATA     #REQUIRED" << std::endl
	<< "          nphi       CDATA     #REQUIRED>" << std::endl
	<< "<!ELEMENT AEndcapCalorimeter EMPTY >" << std::endl
	<< "<!ATTLIST AEndcapCalorimeter" << std::endl
	<< "          c          CDATA     #REQUIRED" << std::endl
	<< "          n          CDATA     #REQUIRED" << std::endl
	<< "          sampling   CDATA     #REQUIRED" << std::endl
	<< "          region     CDATA     #REQUIRED" << std::endl
	<< "          rMin       CDATA     #REQUIRED" << std::endl
	<< "          rMax       CDATA     #REQUIRED" << std::endl
	<< "          zMin       CDATA     #REQUIRED" << std::endl
	<< "          zMax       CDATA     #REQUIRED" << std::endl
	<< "          eta0       CDATA     #REQUIRED" << std::endl
	<< "          deta       CDATA     #REQUIRED" << std::endl
	<< "          neta       CDATA     #REQUIRED" << std::endl
	<< "          meta       CDATA     #REQUIRED" << std::endl
	<< "          phi0       CDATA     #REQUIRED" << std::endl
	<< "          nphi       CDATA     #REQUIRED>" << std::endl
	<< "<!ELEMENT AGapCalorimeter EMPTY >" << std::endl
	<< "<!ATTLIST AGapCalorimeter" << std::endl
	<< "          c          CDATA     #REQUIRED" << std::endl
	<< "          n          CDATA     #REQUIRED" << std::endl
	<< "          sampling   CDATA     #REQUIRED" << std::endl
	<< "          region     CDATA     #REQUIRED" << std::endl
	<< "          rMin       CDATA     #REQUIRED" << std::endl
	<< "          rMax       CDATA     #REQUIRED" << std::endl
	<< "          zMin       CDATA     #REQUIRED" << std::endl
	<< "          zMax       CDATA     #REQUIRED" << std::endl
	<< "          eta        CDATA     #REQUIRED" << std::endl
	<< "          phi0       CDATA     #REQUIRED" << std::endl
	<< "          nphi       CDATA     #REQUIRED>" << std::endl
	<< "<!ELEMENT AEndcapCryostat EMPTY >" << std::endl
	<< "<!ATTLIST AEndcapCryostat" << std::endl
	<< "          c          CDATA     #REQUIRED" << std::endl
	<< "          n          CDATA     #REQUIRED" << std::endl
	<< "          sampling   CDATA     #REQUIRED" << std::endl
	<< "          region     CDATA     #REQUIRED" << std::endl
	<< "          rMin       CDATA     #REQUIRED" << std::endl
	<< "          rMax       CDATA     #REQUIRED" << std::endl
	<< "          zMin       CDATA     #REQUIRED" << std::endl
	<< "          zMax       CDATA     #REQUIRED" << std::endl
	<< "          neta       CDATA     #REQUIRED" << std::endl
	<< "          nphi       CDATA     #REQUIRED>" << std::endl
	<< "<!ELEMENT ABarrelSiliconDetector EMPTY >" << std::endl
	<< "<!ATTLIST ABarrelSiliconDetector" << std::endl
	<< "          c          CDATA     #REQUIRED" << std::endl
	<< "          n          CDATA     #REQUIRED" << std::endl
	<< "          layer      CDATA     #REQUIRED" << std::endl
	<< "          length     CDATA     #REQUIRED" << std::endl
	<< "          width      CDATA     #REQUIRED" << std::endl
	<< "          thickness  CDATA     #REQUIRED" << std::endl
	<< "          tilt       CDATA     #REQUIRED" << std::endl
	<< "          nz         CDATA     #REQUIRED" << std::endl
	<< "          nphi       CDATA     #REQUIRED" << std::endl
	<< "          r0         CDATA     #REQUIRED" << std::endl
	<< "          phi0       CDATA     #REQUIRED" << std::endl
	<< "          zMin       CDATA     #REQUIRED" << std::endl
	<< "          zMax       CDATA     #REQUIRED>" << std::endl
	<< "<!ELEMENT AEndcapSiliconDetector EMPTY >" << std::endl
	<< "<!ATTLIST AEndcapSiliconDetector" << std::endl
	<< "          c          CDATA     #REQUIRED" << std::endl
	<< "          n          CDATA     #REQUIRED" << std::endl
	<< "          layer      CDATA     #REQUIRED" << std::endl
	<< "          length     CDATA     #REQUIRED" << std::endl
	<< "          width      CDATA     #REQUIRED" << std::endl
	<< "          thickness  CDATA     #REQUIRED" << std::endl
	<< "          nz         CDATA     #REQUIRED" << std::endl
	<< "          nphi       CDATA     #REQUIRED" << std::endl
	<< "          rMin       CDATA     #REQUIRED" << std::endl
	<< "          rMax       CDATA     #REQUIRED" << std::endl
	<< "          phi0       CDATA     #REQUIRED" << std::endl
	<< "          zMin       CDATA     #REQUIRED" << std::endl
	<< "          zMax       CDATA     #REQUIRED>" << std::endl
	<< "<!ELEMENT ABarrelTRTDetector EMPTY >" << std::endl
	<< "<!ATTLIST ABarrelTRTDetector" << std::endl
	<< "          c          CDATA     #REQUIRED" << std::endl
	<< "          n          CDATA     #REQUIRED" << std::endl
	<< "          layer      CDATA     #REQUIRED" << std::endl
	<< "          nphi       CDATA     #REQUIRED" << std::endl
	<< "          rMin       CDATA     #REQUIRED" << std::endl
	<< "          rMax       CDATA     #REQUIRED" << std::endl
	<< "          phiIn      CDATA     #REQUIRED" << std::endl
	<< "          phiOut     CDATA     #REQUIRED" << std::endl
	<< "          zMin       CDATA     #REQUIRED" << std::endl
	<< "          zMax       CDATA     #REQUIRED>" << std::endl
	<< "<!ELEMENT AEndcapTRTDetector EMPTY >" << std::endl
	<< "<!ATTLIST AEndcapTRTDetector" << std::endl
	<< "          c          CDATA     #REQUIRED" << std::endl
	<< "          n          CDATA     #REQUIRED" << std::endl
	<< "          layer      CDATA     #REQUIRED" << std::endl
	<< "          nphi       CDATA     #REQUIRED" << std::endl
	<< "          rMin       CDATA     #REQUIRED" << std::endl
	<< "          rMax       CDATA     #REQUIRED" << std::endl
	<< "          phi0       CDATA     #REQUIRED" << std::endl
	<< "          zMin       CDATA     #REQUIRED" << std::endl
	<< "          zMax       CDATA     #REQUIRED>" << std::endl
	<< "]>" << std::endl
	<< "<AGeometry>" << std::endl;
  }

  void GeometryWriter::writePixelGeometry(std::ofstream &out) {

    InDetDD::SiDetectorElementCollection::const_iterator it;

    for (it=m_pixel_manager->getDetectorElementBegin(); it<m_pixel_manager->getDetectorElementEnd(); it++) {

      const InDetDD::SiDetectorElement *element = *it;

      // Just an extra precaution to avoid crashes.
      if (!element) continue;

      if (element->isBarrel() && m_pixelIdHelper->phi_module(element->identify()) == 0  
	  && m_pixelIdHelper->eta_module(element->identify()) == m_pixelIdHelper->eta_module_min(element->identify())) {

	double rMin = element->rMin();
	double rMax = element->rMax();
	double zMax = element->zMax();
	const InDetDD::SiDetectorElement *next = element->nextInEta();
	while (next) {
	  if (next->rMin() < rMin) rMin = next->rMin();
	  if (next->rMax() > rMax) rMax = next->rMax();
	  if (next->zMax() > zMax) zMax = next->zMax();
	  next = next->nextInEta();
	}
	zMax += 5;

	out << "<ABarrelSiliconDetector c=\"" << "PIX" << "\" n=\"" << "Pixel" << "\""
	    << " layer=\"" << m_pixelIdHelper->layer_disk(element->identify()) << "\""
	    << " length=\"" << element->length()/10. << "\""
	    << " width=\"" << element->width()/10. << "\""
	    << " thickness=\"" << (rMax - rMin - element->sinTilt()*element->width())/40. << "\""
	    << " tilt=\"" << 180./M_PI * asin(element->sinTilt()) << "\""
	    << " nz=\"" << m_pixelIdHelper->eta_module_max(element->identify()) 
	  - m_pixelIdHelper->eta_module_min(element->identify())+1 << "\""
	    << " nphi=\"" << m_pixelIdHelper->phi_module_max(element->identify())+1 << "\""
	    << " r0=\"" << (rMin+rMax)/20. << "\""
	    << " phi0=\"" << 180./M_PI * (element->phiMin()+element->phiMax())/2. << "\""
	    << " zMin=\"" << -zMax/10. << "\""
	    << " zMax=\"" << zMax/10. << "\""
	    << " />" << std::endl;
        //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Pixel barrel out: " << out << endmsg;
      }

      if (element->isEndcap() && element->zMin() > 0 && m_pixelIdHelper->phi_module(element->identify()) == 0  
	  && m_pixelIdHelper->eta_module(element->identify()) == m_pixelIdHelper->eta_module_min(element->identify())) {

	double rMin = element->rMin();
	double rMax = element->rMax();
	double zMin = element->zMin();
	double zMax = element->zMax();
	const InDetDD::SiDetectorElement *next = element->nextInEta();
	while (next) {
	  if (next->zMin() < 0) continue;
	  if (next->rMin() < rMin) rMin = next->rMin();
	  if (next->rMax() > rMax) rMax = next->rMax();
	  if (next->zMin() < zMin) zMin = next->zMin();
	  if (next->zMax() > zMax) zMax = next->zMax();
	  next = next->nextInEta();
	}
	zMin -= 5;
	zMax += 5;

	out << "<AEndcapSiliconDetector c=\"" << "PIX" << "\" n=\"" << "Pixel" << "\""
	    << " layer=\"" << m_pixelIdHelper->layer_disk(element->identify()) << "\""
	    << " length=\"" << element->length()/10. << "\""
	    << " width=\"" << element->width()/10. << "\""
	    << " thickness=\"" << (rMax - rMin - element->sinTilt()*element->width())/40. << "\""
	    << " nz=\"" << m_pixelIdHelper->eta_module_max(element->identify()) 
	  - m_pixelIdHelper->eta_module_min(element->identify())+1 << "\""
	    << " nphi=\"" << m_pixelIdHelper->phi_module_max(element->identify())+1 << "\""
	    << " rMin=\"" << rMin/10. << "\""
	    << " rMax=\"" << rMax/10. << "\""
	    << " phi0=\"" << 180./M_PI * (element->phiMin()+element->phiMax())/2. << "\""
	    << " zMin=\"" << zMin/10. << "\""
	    << " zMax=\"" << zMax/10. << "\""
	    << " />" << std::endl;
        //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Pixel endcap out: " << out << endmsg;
      }
    }
  }

  void GeometryWriter::writeSCTGeometry(std::ofstream &out) {

    InDetDD::SiDetectorElementCollection::const_iterator it;
 
    for (it=m_silicon_manager->getDetectorElementBegin(); it<m_silicon_manager->getDetectorElementEnd(); it++) {

      const InDetDD::SiDetectorElement *element = *it;

      // Just an extra precaution to avoid crashes.
      if (!element) continue;

      if (element->isBarrel() && element->zMax() > 0 && m_sctIdHelper->phi_module(element->identify()) == 0  
	  && m_sctIdHelper->eta_module(element->identify()) == m_sctIdHelper->eta_module_min(element->identify())
	  && m_sctIdHelper->side(element->identify()) > 0) {

	double rMin = element->rMin();
	double rMax = element->rMax();
	double zMax = element->zMax();
	const InDetDD::SiDetectorElement *next = element->nextInEta();
	while (next) {
	  if (next->rMin() < rMin) rMin = next->rMin();
	  if (next->rMax() > rMax) rMax = next->rMax();
	  if (next->zMax() > zMax) zMax = next->zMax();
	  next = next->nextInEta();
	}
	next = element->otherSide();
	while (next) {
	  if (next->rMin() < rMin) rMin = next->rMin();
	  if (next->rMax() > rMax) rMax = next->rMax();
	  if (next->zMax() > zMax) zMax = next->zMax();
	  next = next->nextInEta();
	}
	zMax += 5;

	out << "<ABarrelSiliconDetector c=\"" << "SIL" << "\" n=\"" << "Silicon" << "\""
	    << " layer=\"" << m_sctIdHelper->layer_disk(element->identify()) << "\""
	    << " length=\"" << element->length()/10. << "\""
	    << " width=\"" << element->width()/10. << "\""
	    << " thickness=\"" << 2. * (rMax - rMin - element->sinTilt()*element->width())/10. << "\""
	    << " tilt=\"" << 180./M_PI * asin(element->sinTilt()) << "\""
	    << " nz=\"" << m_sctIdHelper->eta_module_max(element->identify()) 
	  - m_sctIdHelper->eta_module_min(element->identify())+1 << "\""
	    << " nphi=\"" << m_sctIdHelper->phi_module_max(element->identify())+1 << "\""
	    << " r0=\"" << (rMin+rMax)/20. << "\""
	    << " phi0=\"" << 180./M_PI * (element->phiMin()+element->phiMax())/2. << "\""
	    << " zMin=\"" << -zMax/10. << "\""
	    << " zMax=\"" << zMax/10. << "\""
	    << " />" << std::endl;
        //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "SCT barrel out: " << out << endmsg;
      }

      if (element->isEndcap() && element->zMin() > 0 && m_sctIdHelper->phi_module(element->identify()) == 0  
	  && m_sctIdHelper->eta_module(element->identify()) == m_sctIdHelper->eta_module_min(element->identify())
	  && m_sctIdHelper->side(element->identify()) > 0) {

	double rMin = element->rMin();
	double rMax = element->rMax();
	double zMin = element->zMin();
	double zMax = element->zMax();
	const InDetDD::SiDetectorElement *next = element->nextInEta();
	while (next) {
	  if (next->zMin() < 0) continue;
	  if (next->rMin() < rMin) rMin = next->rMin();
	  if (next->rMax() > rMax) rMax = next->rMax();
	  if (next->zMin() < zMin) zMin = next->zMin();
	  if (next->zMax() > zMax) zMax = next->zMax();
	  next = next->nextInEta();
	}
	next = element->otherSide();
	while (next) {
	  if (next->zMin() < 0) continue;
	  if (next->rMin() < rMin) rMin = next->rMin();
	  if (next->rMax() > rMax) rMax = next->rMax();
	  if (next->zMin() < zMin) zMin = next->zMin();
	  if (next->zMax() > zMax) zMax = next->zMax();
	  next = next->nextInEta();
	}
	zMin -= 5;
	zMax += 5;

	out << "<AEndcapSiliconDetector c=\"" << "SIL" << "\" n=\"" << "Silicon" << "\""
	    << " layer=\"" << m_sctIdHelper->layer_disk(element->identify()) << "\""
	    << " length=\"" << element->length()/10. << "\""
	    << " width=\"" << element->width()/10. << "\""
	    << " thickness=\"" << 2. * (rMax - rMin - element->sinTilt()*element->width())/10. << "\""
	    << " nz=\"" << m_sctIdHelper->eta_module_max(element->identify()) 
	  - m_sctIdHelper->eta_module_min(element->identify())+1 << "\""
	    << " nphi=\"" << m_sctIdHelper->phi_module_max(element->identify())+1 << "\""
	    << " rMin=\"" << rMin/10. << "\""
	    << " rMax=\"" << rMax/10. << "\""
	    << " phi0=\"" << 180./M_PI * (element->phiMin()+element->phiMax())/2. << "\""
	    << " zMin=\"" << zMin/10. << "\""
	    << " zMax=\"" << zMax/10. << "\""
	    << " />" << std::endl;
        //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "SCT endcap out: " << out << endmsg;
      }
    }
  }

  void GeometryWriter::writeTRTGeometry(std::ofstream &out) {

    InDetDD::TRT_DetElementCollection::const_iterator it;
    
    for (it=m_trt_manager->getDetectorElementBegin(); it<m_trt_manager->getDetectorElementEnd(); it++) {
      
      const InDetDD::TRT_BaseElement *elementIn = *it;
      // Just an extra precaution to avoid crashes.
      if (!elementIn) continue;
      Identifier id = elementIn->identify();
      
      if (m_trtIdHelper->phi_module(id) != 0) continue;
      if (m_trtIdHelper->straw_layer(id) != 0) continue;
      Amg::Vector3D posIn = elementIn->strawTransform(0) * Amg::Vector3D(0., 0., 0.);
      if (posIn.z() < 0) continue;

      const InDetDD::TRT_BaseElement *elementOut;

      if (m_trtIdHelper->is_barrel(id)) {
	elementOut = m_trt_manager->getBarrelElement(m_trtIdHelper->barrel_ec(id),
						     m_trtIdHelper->layer_or_wheel(id),
						     0,
						     m_trtIdHelper->straw_layer_max(id));

	// Just an extra precaution to avoid crashes.
	if (!elementOut) continue;

	Amg::Vector3D posOut = elementOut->strawTransform(0) * Amg::Vector3D(0.,0.,0.);
	//	HepGeom::Point3D<double> posOut = elementOut->strawTransform(0) * HepGeom::Point3D<double>(0., 0., 0.);

	const InDetDD::TRT_BarrelElement *element = dynamic_cast<const InDetDD::TRT_BarrelElement *>(elementIn);
        if (!element) continue;
	int nphi = (int) round(2.*M_PI / fabs(element->nextInPhi()->center().phi() - element->center().phi()));
	double dphiIn = 2.*M_PI / (nphi * m_trtIdHelper->straw_max(elementIn->identify()));
	double dphiOut = 2.*M_PI / (nphi * m_trtIdHelper->straw_max(elementOut->identify()));

	out << "<ABarrelTRTDetector c=\"TRT\" n=\"TRT\""
	    << " layer=\"" << m_trtIdHelper->layer_or_wheel(id) << "\""
	    << " nphi=\"" << nphi << "\""
	    << " rMin=\"" << posIn.perp()/10. - .4 << "\""
	    << " rMax=\"" << posOut.perp()/10. + .4 << "\""
	    << " phiIn=\"" << 180./M_PI * (posIn.phi() - dphiIn - 2e-3) << "\""
	    << " phiOut=\"" << 180./M_PI * (posOut.phi() - dphiOut - 2e-3) << "\""
	    << " zMin=\"" << -(posIn.z() + elementIn->strawLength()/2.)/10. << "\""
	    << " zMax=\"" << (posIn.z() + elementIn->strawLength()/2.)/10. << "\""
	    << " />" << std::endl;
        //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TRT barrel out: " << out << endmsg;
      } else {
	elementOut = m_trt_manager->getEndcapElement(1,//m_trtIdHelper->barrel_ec(id),
						     m_trtIdHelper->layer_or_wheel(id),
						     m_trtIdHelper->straw_layer_max(id),
						     0);

	// Just an extra precaution to avoid crashes.
	if (!elementOut) continue;

	//HepGeom::Point3D<double> posOut = elementOut->strawTransform(m_trtIdHelper->straw_max(id)) * HepGeom::Point3D<double>(0., 0., 0.);
	Amg::Vector3D posOut = elementOut->strawTransform(m_trtIdHelper->straw_max(id)) * Amg::Vector3D(0.,0.,0.);

	// floor() instead of round() becuase we are neglecting the space between two modules
 	int nphi = (int) floor(2.*M_PI / fabs(posOut.phi() - posIn.phi()));

	out << "<AEndcapTRTDetector c=\"TRT\" n=\"TRT\""
	    << " layer=\"" << m_trtIdHelper->layer_or_wheel(id) << "\""
	    << " nphi=\"" << nphi << "\""
	    << " rMin=\"" << (posIn.perp()-elementIn->strawLength()/2.)/10. << "\""
	    << " rMax=\"" << (posIn.perp()+elementIn->strawLength()/2.)/10. << "\""
	    << " phi0=\"" << 180./M_PI * posIn.phi() << "\""
	    << " zMin=\"" << posIn.z()/10. << "\""
	    << " zMax=\"" << posOut.z()/10. << "\""
	    << " />" << std::endl;
        //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TRT endcap out: " << out << endmsg;
      }

    }
  }

  void GeometryWriter::writeSolenoidGeometry(std::ofstream &out) {

    double zIn = -265.0;
    double zOut = 265.0;
    double rIn = 122.9;
    double rOut = 127.4;

    out << "<ADisc p=\"YX\" c=\"Sol\"  n=\"Solenoid\"" 
	<< " rIn=\"" << rIn << "\""
	<< " rOut=\"" << rOut << "\" />"
	<< std::endl;

    out << "<ARectangle p=\"RZ\" c=\"Sol\" n=\"Solenoid\""
	<< " xMin=\"" << zIn << "\""
	<< " xMax=\"" << zOut << "\""
	<< " yMin=\"" << rIn << "\""
	<< " yMax=\"" << rOut << "\""
	<< " xR=\"NO\" />"
	<< std::endl;
  }


  void GeometryWriter::writeTILEGeometry(std::ofstream &out) {

    const TileID *tileIdHelper = m_tile_manager->get_id();
    TileDetDescrManager::tile_descr_const_iterator tileIt;
    for (tileIt=m_tile_manager->tile_descriptors_begin(); tileIt!=m_tile_manager->tile_descriptors_end(); tileIt++) {
      const TileDetDescriptor *descriptor = *tileIt;
      if (!descriptor) continue;

      int i;
      for (i=0; i<descriptor->n_samp(); i++) {

	if (descriptor->zcenter(i) <= 0) continue;

	std::string name;
	if (tileIdHelper->is_tile_barrel(descriptor->identify())) name = "TILE Barrel";
	else if (tileIdHelper->is_tile_extbarrel(descriptor->identify())) name = "Extended TILE";
	else if (tileIdHelper->is_tile_gap(descriptor->identify())) name = "ITC Gap";
	else if (tileIdHelper->is_tile_gapscin(descriptor->identify())) name = "ITC Gap Scintillator";
	else name = "TILE";

	if (!name.compare(0, 3, "ITC")) {

	  out << "<AGapCalorimeter c=\"" << "HCAL" << "\" n=\"" << name << "\""
	      << " sampling=\"" << (i < 3 ? i : 3)  << "\" region=\"" << 0 << "\""
	      << " rMin=\"" << (descriptor->rcenter(i)-descriptor->dr(i)/2.)/10. << "\""
	      << " rMax=\"" << (descriptor->rcenter(i)+descriptor->dr(i)/2.)/10. << "\""
	      << " zMin=\"" << (descriptor->zcenter(i)-descriptor->dz(i)/2.)/10. << "\""
	      << " zMax=\"" << (descriptor->zcenter(i)+descriptor->dz(i)/2.)/10. << "\""
	      << " eta=\"" << (i < 3 ? tileIdHelper->eta_min(descriptor->identify())-i+2 : i) << "\""
	      << " phi0=\"" << descriptor->phi_min() << "\""
	      << " nphi=\"" << descriptor->n_phi() << "\""
	      << " />" << std::endl;
	} else {
	  int section = tileIdHelper->section(descriptor->identify());
	  int side = tileIdHelper->side(descriptor->identify());
	  int module = tileIdHelper->module(descriptor->identify());
	  int tower = tileIdHelper->tower(descriptor->identify());
	  Identifier id = tileIdHelper->cell_id(section, side, module, tower, i);

	  out << "<ABarrelCalorimeter c=\"" << "HCAL" << "\" n=\"" << name << "\""
	      << " sampling=\"" << i << "\" region=\"" << 0 << "\""
	      << " rMin=\"" << (descriptor->rcenter(i)-descriptor->dr(i)/2.)/10. << "\""
	      << " rMax=\"" << (descriptor->rcenter(i)+descriptor->dr(i)/2.)/10. << "\""
	      << " zMin=\"" << (descriptor->zcenter(i)-descriptor->dz(i)/2.)/10. << "\""
	      << " zMax=\"" << (descriptor->zcenter(i)+descriptor->dz(i)/2.)/10. << "\""
	      << " eta0=\"" << descriptor->eta_min(i) << "\""
	      << " deta=\"" << descriptor->deta(i) << "\""
	      << " neta=\"" << descriptor->n_eta(i) << "\""
	      << " meta=\"" << tileIdHelper->eta_min(id) << "\""
	      << " phi0=\"" << descriptor->phi_min() << "\""
	      << " nphi=\"" << descriptor->n_phi() << "\""
	      << " />" << std::endl;
	}
      }
    }
  }

  void GeometryWriter::writeLArGeometry(std::ofstream &out, const CaloDetDescrManager* caloMgr) {

    const CaloCell_ID *idHelper = caloMgr->getCaloCell_ID();
    CaloDetDescrManager::calo_element_const_iterator it;

    // This code is not very efficient in terms of speed. Since it will only be used 
    // when the geometry has changed, the code is made to be easily readable instead.
    for (int type=0; type<m_numCaloTypes; type++) {

      for (int region=0; region<8; region++) {

	for (int inner=0; inner<2; inner++) {

	  int minEtaIndex = 0, maxEtaIndex = 0;
	  double minEta = 0.0;
	  const CaloDetDescrElement *oneSuchElement = 0;

	  for (it=caloMgr->element_begin(); it<caloMgr->element_end(); it++) {

	    const CaloDetDescrElement *element = *it;
	    int etaIndex, phiIndex;
	    std::string technology;
	    std::string name;

	    if (!element) continue;

	    Identifier id = element->identify();

	    // Select everything but TILE.
	    if (element->is_tile()) continue;

	    // Select only this sampling.
	    if (element->getSampling() != type) continue;

	    // Select only this region.
	    if (element->is_lar_fcal()) {
	      if (idHelper->eta(id) != idHelper->eta_min(id) || idHelper->side(id) < 0) continue;
	    } else {
	      if (idHelper->region(id) != region) continue;
	    }

	    // Differentiate between the LAr inner/outer endcap
	    if (inner > 0 && !idHelper->is_em_endcap_inner(id)) continue;

	    // Skip -z, we will mirror +z.
	    if (element->z() < 0) continue;

	    // Select only the first element in phi.
	    etaIndex = idHelper->eta(id);
	    phiIndex = idHelper->phi(id);
	    if (phiIndex > 0) continue;

	    if (!oneSuchElement) {
	      oneSuchElement = element;
	      minEtaIndex = maxEtaIndex = etaIndex;
	      minEta = element->eta();
	    } else {
	      if (etaIndex < minEtaIndex) {
		minEtaIndex = etaIndex;
		minEta = element->eta();
	      }
	      if (etaIndex > maxEtaIndex) {
		maxEtaIndex = etaIndex;
	      }
	    }
	  }

	  if (oneSuchElement) {

	    std::string color;
	    std::string name;
	    int sampling = 0;
	    enum {BARREL, ENDCAP, FORWARD} part = BARREL;
	    
	    const CaloDetDescriptor *descriptor = oneSuchElement->descriptor();

	    switch(type) {
	    case CaloCell_ID::PreSamplerB:
	      color = "ECAL";
	      name = "LAr Presampler";
	      sampling = 0;
	      part = BARREL;
	      break;
	    case CaloCell_ID::EMB1:
	    case CaloCell_ID::EMB2:
	    case CaloCell_ID::EMB3:
	      color = "ECAL";
	      name = "LAr";
	      sampling = type - CaloCell_ID::PreSamplerB;
	      part = BARREL;
	      break;
	    case CaloCell_ID::PreSamplerE:
	      color = "ECAL";
	      name = "LAr_EC_Presampler";
	      part = ENDCAP;
	      break;
	    case CaloCell_ID::EME1:
	    case CaloCell_ID::EME2:
	    case CaloCell_ID::EME3:
	      color = "ECAL";
	      name = inner ? "LAr Inner Endcap" : "LAr Outer Endcap";
	      sampling = type - CaloCell_ID::PreSamplerE;
	      part = ENDCAP;
	      break;
	    case CaloCell_ID::HEC0:
	    case CaloCell_ID::HEC1:
	    case CaloCell_ID::HEC2:
	    case CaloCell_ID::HEC3:
	      color = "HCAL";
	      name = "HEC";
	      sampling = type - CaloCell_ID::HEC0;
	      part = ENDCAP;
	      break;
	    case CaloCell_ID::FCAL0:
	      color = "ECAL";
	      name = "FCAL EM";
	      part = FORWARD;
	      break;
	    case CaloCell_ID::FCAL1:
	      color = "HCAL";
	      name = "FCAL HAD 1";
	      part = FORWARD;
	      break;
	    case CaloCell_ID::FCAL2:
	      color = "HCAL";
	      name = "FCAL HAD 2";
	      part = FORWARD;
	      break;
	    }

	    double phi0 = 180./M_PI * (oneSuchElement->phi()-oneSuchElement->dphi()/2.);
	    int numPhi = (int) round(2.*M_PI / oneSuchElement->dphi());
	    int numEta = maxEtaIndex - minEtaIndex + 1;

	    switch (part) {
	    case BARREL:
	      out << "<ABarrelCalorimeter c=\"" << color << "\" n=\"" << name << "\""
		  << " sampling=\"" << sampling << "\" region=\"" << region%8 << "\""
		  << " rMin=\"" << (oneSuchElement->r()-oneSuchElement->dr()/2.)/10. << "\""
		  << " rMax=\"" << (oneSuchElement->r()+oneSuchElement->dr()/2.)/10. << "\""
		  << " zMin=\"" << m_larMin[type] << "\""
		  << " zMax=\"" << m_larMax[type] << "\""
		  << " eta0=\"" << minEta-oneSuchElement->deta()/2. << "\""
		  << " deta=\"" << oneSuchElement->deta() << "\""
		  << " neta=\"" << numEta << "\""
		  << " meta=\"" << minEtaIndex << "\""
		  << " phi0=\"" << phi0 << "\""
		  << " nphi=\"" << numPhi << "\""
		  << " />" << std::endl;
	      break;
	    case ENDCAP:
	      out << "<AEndcapCalorimeter c=\"" << color << "\" n=\"" << name << "\""
		  << " sampling=\"" << sampling << "\" region=\"" << region%8 << "\""
		  << " rMin=\"" << m_larMin[type] << "\""
		  << " rMax=\"" << m_larMax[type] << "\""
		  << " zMin=\"" << (oneSuchElement->z()-oneSuchElement->dz()/2.)/10. << "\""
		  << " zMax=\"" << (oneSuchElement->z()+oneSuchElement->dz()/2.)/10. << "\""
		  << " eta0=\"" << minEta-oneSuchElement->deta()/2. << "\""
		  << " deta=\"" << oneSuchElement->deta() << "\""
		  << " neta=\"" << numEta << "\""
		  << " meta=\"" << minEtaIndex << "\""
		  << " phi0=\"" << phi0 << "\""
		  << " nphi=\"" << numPhi << "\""
		  << " />" << std::endl;
	      break;
	    case FORWARD:
	      out << "<ADisc p=\"YX\" c=\"" << color << "\" n=\"" << name << "\""
		  << " rIn=\"" << descriptor->calo_r_min()/10. << "\""
		  << " rOut=\"" << descriptor->calo_r_max()/10. << "\""
		  << " />" << std::endl;
	      out << "<ARectangle p=\"RZ\" c=\"" << color << "\" n=\"" << name << "\""
		  << " xMin=\"" << descriptor->calo_z_min()/10. << "\""
		  << " xMax=\"" << descriptor->calo_z_max()/10. << "\""
		  << " yMin=\"" << descriptor->calo_r_min()/10. << "\""
		  << " yMax=\"" << descriptor->calo_r_max()/10. << "\""
		  << " />" << std::endl;
	    }
	  }
	}
      }
    }
  }
  
  void GeometryWriter::writeMBTSGeometry(std::ofstream &out) {
	 //  volume hierarchy tree leading from MBTS_mother to scintillators:
	 //  MBTS_mother                                                                                                  
	 //    MBTSAirEnv <--- 8 copies                                                                               
	 //      MBTSAluEnv                                                                                               
	 //        MBTSAirInAlu                                                                                           
	 //          MBTS1                                                                                                
	 //          MBTS2 
	 //          MBTSPlug2In <--- skip this one

	 PVConstLink myVol = m_lar_manager->getTreeTop(1U);
	 for (unsigned int c=0; c< myVol->getNChildVols();c++) 
	 {
		PVConstLink child = myVol->getChildVol(c);
		if((child->getLogVol())->getName()=="MBTS_mother")
		{
		   int sampling=0,numPhi=0;
		   std::string stringOfNames="";

		   GeoTrf::Vector3D translate = (child->getX()).translation(); // vector from origin to MBTS_mother

		   double zlocation = translate.z();
		   if(zlocation<0) 
			  zlocation=-zlocation;

		   GeoTrf::Transform3D transformToScin; // for calculating the vector from MBTS_mother to MBTS scintilators

		   PVConstLink pvAirEnv = 0;
		   for (unsigned int cc=0; cc< child->getNChildVols();cc++) {
			  if(((child->getChildVol(cc))->getLogVol())->getName()=="MBTSAirEnv") {
				 pvAirEnv = child->getChildVol(cc);
				 transformToScin = child->getXToChildVol(cc);
				 numPhi++;
			  }
		   } 
		   if(!pvAirEnv) {
			  if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not find MBTSAirEnv" << endmsg;
			  return;
		   }

		   PVConstLink pvAluEnv = 0;
		   for (unsigned int ichildAirEnv=0; ichildAirEnv<pvAirEnv->getNChildVols(); ichildAirEnv++) {
			  if(((pvAirEnv->getChildVol(ichildAirEnv))->getLogVol())->getName()=="MBTSAluEnv") {
				 pvAluEnv = pvAirEnv->getChildVol(ichildAirEnv);
				 transformToScin = transformToScin * pvAirEnv->getXToChildVol(ichildAirEnv);
			  }
		   } 
		   if(!pvAluEnv) {
			  if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not find MBTSAluEnv" << endmsg;
			  return;
		   }

		   PVConstLink pvAirInAlu = 0;
		   for (unsigned int ichildAluEnv=0; ichildAluEnv<pvAluEnv->getNChildVols(); ichildAluEnv++) {
			  if(((pvAluEnv->getChildVol(ichildAluEnv))->getLogVol())->getName()=="MBTSAirInAlu") {
				 pvAirInAlu = pvAluEnv->getChildVol(ichildAluEnv);
				 transformToScin = transformToScin * pvAluEnv->getXToChildVol(ichildAluEnv);
			  }
		   }
		   if(!pvAirInAlu) {
			  if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not find MBTSAirInAlu" << endmsg;
			  return;
		   }

		   for (unsigned int ichildAirInAlu=0; ichildAirInAlu<pvAirInAlu->getNChildVols(); ichildAirInAlu++) {
			  PVConstLink childschild = pvAirInAlu->getChildVol(ichildAirInAlu);
			  if((childschild->getLogVol())->getShape()->typeID() == GeoTrd::getClassTypeID() )
			  {
				 std::string currentName = (childschild->getLogVol())->getName();
				 if(currentName!="MBTS1" && currentName!="MBTS2") continue;
				 if(stringOfNames.find(currentName,0) == std::string::npos)
				 {
					stringOfNames+=" " + currentName;
					GeoTrf::Vector3D translateToScin = (transformToScin * pvAirInAlu->getXToChildVol(ichildAirInAlu)).translation();
					const GeoTrd* theTrd = dynamic_cast<const GeoTrd*> ((childschild->getLogVol())->getShape());
					double rho=pow(translateToScin.x(),2.0) + pow(translateToScin.y(),2.0);
					rho=pow(rho,0.5);
					double RMin=rho-theTrd->getZHalfLength();
					double RMax=rho+theTrd->getZHalfLength();
					double zmovement=translateToScin.z();
					double zthickness=theTrd->getXHalfLength1();
					out << "<AEndcapCryostat c=\"HCAL\" n=\"Minimum Bias Trigger Scintillators\""
					   << " sampling=\"" << sampling << "\" region=\"" << 0 << "\""
					   << " rMin=\"" << RMin/10. << "\""
					   << " rMax=\"" << RMax/10. << "\""
					   << " zMin=\"" << (zlocation+zmovement-zthickness)/10. << "\""
					   << " zMax=\"" << (zlocation+zmovement+zthickness)/10. << "\""
					   << " neta=\"" << "1" << "\""
					   << " nphi=\"" << numPhi << "\""
					   << " />" << std::endl;
					sampling++;
				 } 
			  }
		   }


		}
	 }
  }

  void GeometryWriter::writeFooter(std::ofstream &out) {

    out << "</AGeometry>" << std::endl;
  }
}
