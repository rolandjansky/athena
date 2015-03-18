/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelDistortionsTestReadWrite.h"


// Athena includes
//#include "StoreGate/StoreGateSvc.h"

// Gaudi includes
#include "GaudiKernel/AlgFactory.h"

#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "PixelConditionsTools/IModuleDistortionsTool.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"

PixelDistortionsTestReadWrite::PixelDistortionsTestReadWrite(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_detManager(0),
  m_pixelDistoTool("PixelDistortionsTool")
{

  //m_log.setLevel(outputLevel());
  declareProperty("FullTest",m_fullTest=true);

}

  
PixelDistortionsTestReadWrite::~PixelDistortionsTestReadWrite()
{}


StatusCode PixelDistortionsTestReadWrite::initialize()
{
  //m_log.setLevel(outputLevel());
  if (msgLvl(MSG::INFO))msg(MSG::INFO) << "Entering PixelDistortionsTestReadWrite::initialize()" << endreq;

  StatusCode sc;

  const std::string managerName = "Pixel";
  sc=detStore()->retrieve(m_detManager,managerName);
  if (sc.isFailure() || !m_detManager) {
    if (msgLvl(MSG::FATAL))msg(MSG::FATAL) << "Could not find the Detector Manager: "
	<< managerName << " !" << endreq;
    return StatusCode::FAILURE;
  } else {
    if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Detector Manager found" << endreq;
  }

  
  // Get the tool
  if (!m_pixelDistoTool.empty()) {
    sc = m_pixelDistoTool.retrieve();
    if(sc.isFailure()){
      if (msgLvl(MSG::FATAL))msg(MSG::FATAL) << "Unable to retrieve PixelDistortionsTool" << endreq;
      return StatusCode::FAILURE;
    }

    if (msgLvl(MSG::INFO))msg(MSG::INFO) << "PixelDistortionsTool retrieved" << endreq;
  } else {
    if (msgLvl(MSG::INFO))msg(MSG::INFO) << "No PixelDistortionsTool requested." << endreq;
  }


  return StatusCode::SUCCESS;

}


StatusCode PixelDistortionsTestReadWrite::execute(){
  if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << " in PixelDistortionsTestReadWrite::execute()" << endreq;
  if (!m_pixelDistoTool.empty()) {
    if (!m_fullTest) {
      // Do nothing
      *m_pixelDistoTool;
    } else {
      const PixelID * pixelIdHelper;
      if (detStore()->retrieve(pixelIdHelper, "PixelID").isFailure()) {
	  msg(MSG::FATAL) << "Could not get Pixel ID helper" << endreq;
	  return StatusCode::FAILURE;
    }
    // Exercise the tool.
    InDetDD::SiDetectorElementCollection::const_iterator iter;
    
    for (iter = m_detManager->getDetectorElementBegin(); iter != m_detManager->getDetectorElementEnd(); ++iter){
		const InDetDD::SiDetectorElement * element = *iter; 
		if (element) {
		  Identifier id = element->identify();
		  Amg::Vector2D point1(-0.123, 0.456);
		  Amg::Vector2D point2(1.789, -5.678);
		  Amg::Vector3D dir(0,0,1); //unit z direction
		  const Amg::Transform3D rotateAboutX(Amg::AngleAxis3D(10 * CLHEP::deg, Amg::Vector3D::UnitX()));
		  const Amg::Transform3D rotateAboutY(Amg::AngleAxis3D(5 * CLHEP::deg, Amg::Vector3D::UnitY()));
		  const Amg::Transform3D totalTransform = rotateAboutX * rotateAboutY;
		  dir = totalTransform.linear() * dir;
		  //dir.rotateX(10*CLHEP::deg);
		  //dir.rotateY(5*CLHEP::deg);
		  Amg::Vector2D corr;
		  if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << pixelIdHelper->show_to_string(id) << endreq;
		  corr = m_pixelDistoTool->correction(id, point1, dir); 
		  if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << " Point 1: " << "(" << point1[0] << ", " <<  point1[1] << ")"
			<< ", correction: " << "(" << corr.x() << ", " << corr.y() << ")" <<endreq;
		  corr = m_pixelDistoTool->correction(id, point2, dir);
		  if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << " Point 2: " << "(" << point2[0] << ", " <<  point2[1] << ")" 
			<< ", correction: " << "(" << corr.x() << ", " << corr.y() << ")" <<endreq;
		  Amg::Vector2D newPoint2sim = m_pixelDistoTool->correctSimulation(id, point2, dir);
		  if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << " Point 2 after correction in simul: " 
			<<  "(" << newPoint2sim[0] << ", " <<  newPoint2sim[1] << ")" << endreq;
		  Amg::Vector2D newPoint2rec = m_pixelDistoTool->correctReconstruction(id, newPoint2sim, dir);
		  if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << " Point 2 after correction in recon: " 
			<<  "(" << newPoint2rec[0] << ", " <<  newPoint2rec[1] << ")" << endreq;
	  
		}
      }
    }
  }
  return StatusCode::SUCCESS;

}  

StatusCode PixelDistortionsTestReadWrite::finalize(){

  if (msgLvl(MSG::INFO))msg(MSG::INFO) << "in PixelDistortionsTestReadWrite::finalize()" << endreq;
  
  return StatusCode::SUCCESS;
}  
