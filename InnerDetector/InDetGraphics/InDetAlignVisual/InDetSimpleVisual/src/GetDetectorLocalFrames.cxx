/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
    Authors: John Alison <johnda@hep.upenn.edu>
    
    Date: 22 Aug 2008
    
    Description: This algorithm loops over the Inner Detector elements 
    and prints thier global positions to a text file

*/

#include "InDetSimpleVisual/GetDetectorLocalFrames.h"

#include "GaudiKernel/MsgStream.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "TRT_ReadoutGeometry/TRT_Numerology.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "InDetIdentifier/TRT_ID.h"
#include "TRT_ReadoutGeometry/TRT_DetectorManager.h"

#include "EventPrimitives/EventPrimitives.h"

#include "StoreGate/ReadCondHandle.h"

#include <stdlib.h>
#include <string.h>
#include <vector>


/** Constructor */
GetDetectorLocalFrames::GetDetectorLocalFrames(std::string const&  name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_outputFileName("IDLocalFrames.txt"),  
  
  /** Pixel Variables */

  /** SCT variables */  

  /** TRT variables */    
  m_trt_barrel_ec(0),
  m_trt_layer_or_wheel(0),
  m_trt_phi_module(0),
  m_trt_straw_layer(0),
  m_trt_straw(0),

  /** ID Tools */
  m_TRTHelper(0),
  m_TRTDetectorManager(0)

{
  declareProperty("OutputTextFile",m_outputFileName);
}

/** initialize */
StatusCode GetDetectorLocalFrames::initialize(){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "initialize()" << endmsg;
  
  /** Retrive TRT info */
  if (detStore()->retrieve(m_TRTHelper, "TRT_ID").isFailure()) {
    msg(MSG::FATAL) << "Could not get TRT ID helper" << endmsg;
    return StatusCode::FAILURE;
  }
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "got the TRT ID" << endmsg;

  if ((detStore()->retrieve(m_TRTDetectorManager)).isFailure()) {
    if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Problem retrieving TRT_DetectorManager" << endmsg;
    return StatusCode::FAILURE;
  }
  
  /** Retrive SCT info */
  ATH_CHECK(m_SCTDetEleCollKey.initialize());
  
  /** Retrive Pixel info */
  ATH_CHECK(m_pixelDetEleCollKey.initialize());
  
  /** Output text File */
  m_outputFile.open((m_outputFileName).c_str());
  
  return StatusCode::SUCCESS;
}

/** execute */
StatusCode GetDetectorLocalFrames::execute() {
  
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "execute() check global position" << endmsg;
  
    StatusCode sc = StatusCode::SUCCESS;
    m_eventCount++;
    
    /** run only for one event */
    if (m_eventCount!=0) 
      return sc; 
    
    std::cout << "========================================================================================" <<std::endl;
    std::cout << "==================== Getting the Inner Detector Local Frames ===========================" <<std::endl;
    std::cout << "========================================================================================" <<std::endl;

    //Write pixel positions
    writePixelFames();

    //Write SCT positions
    writeSCTFrames();

    //Write SCT positions
    writeTRTFrames();

    return StatusCode::SUCCESS;
}

/** Finalize */
StatusCode GetDetectorLocalFrames::finalize() {
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "finalize()" << endmsg;
  
  /** Close the file */
  m_outputFile.close();

  return StatusCode::SUCCESS;
}

/** Writing the Pixel Positions */
void GetDetectorLocalFrames::writePixelFames(){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writePixelFames()" << endmsg;

  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> pixelDetEleHandle(m_pixelDetEleCollKey);
  const InDetDD::SiDetectorElementCollection* elements{*pixelDetEleHandle};
  if (not pixelDetEleHandle.isValid() or elements==nullptr) {
    ATH_MSG_ERROR(m_pixelDetEleCollKey.fullKey() << " is not available.");
    return;
  }
  //Loop over pixel elements
  //  for (const InDetDD::SiDetectorElement* element: *elements) {
    // Get local Frame
  //  }
  
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Leaving writePixelFames()" << endmsg;
  return;
}

/** Writing the SCT Positions */
void GetDetectorLocalFrames::writeSCTFrames(){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writeSCTFrames()" << endmsg;

  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEleHandle(m_SCTDetEleCollKey);
  const InDetDD::SiDetectorElementCollection* elements{*sctDetEleHandle};
  if (not sctDetEleHandle.isValid() or elements==nullptr) {
    ATH_MSG_ERROR(m_SCTDetEleCollKey.fullKey() << " is not available.");
    return;
  }
  //Loop over SCT elements
  //  for (const InDetDD::SiDetectorElement* element: *elements) {
    // Get local Frame
  //  }
  
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Leaving writeSCTFrames()" << endmsg;
  return;
}

/** Writing the Detailed (all straws) TRT Positions */
void GetDetectorLocalFrames::writeTRTFrames(){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writeTRTFrames()" << endmsg;
  
  TRT_ID::const_expanded_id_iterator trtStrawIt = m_TRTHelper->straw_begin();
  TRT_ID::const_expanded_id_iterator trtStrawItE = m_TRTHelper->straw_end();
  
  for (; trtStrawIt != trtStrawItE; ++trtStrawIt) {
    const ExpandedIdentifier& expId = (*trtStrawIt);
    
    Identifier trtId = m_TRTHelper->straw_id(expId[2], 
					     expId[3], 
					     expId[4],
					     expId[5], 
					     expId[6]);
    
    m_trt_barrel_ec = m_TRTHelper->barrel_ec(trtId);
    m_trt_layer_or_wheel = m_TRTHelper->layer_or_wheel(trtId);
    m_trt_phi_module = m_TRTHelper->phi_module(trtId);
    m_trt_straw_layer = m_TRTHelper->straw_layer(trtId);
    m_trt_straw = m_TRTHelper->straw(trtId);
    
    m_outputFile << 3 << " "
	       << m_trt_barrel_ec << " "
	       << m_trt_layer_or_wheel << " "
	       << m_trt_phi_module << " "
	       << m_trt_straw_layer << " "
	       << m_trt_straw << " ";
    m_outputFile << std::endl;

    //const HepGeom::Transform3D& localTransform = m_TRTDetectorManager->getElement( trtId )->transform(trtId);
    Amg::Vector3D xaxis(1,0,0);
    Amg::Vector3D yaxis(0,1,0);
    Amg::Vector3D zaxis(0,0,1);
    const Amg::Vector3D strawXAxis = m_TRTDetectorManager->getElement( trtId )->strawTransform(m_trt_straw) * xaxis * m_TRTDetectorManager->getElement( trtId )->strawDirection();
    const Amg::Vector3D strawYAxis = m_TRTDetectorManager->getElement( trtId )->strawTransform(m_trt_straw) * yaxis * m_TRTDetectorManager->getElement( trtId )->strawDirection();
    const Amg::Vector3D strawZAxis = m_TRTDetectorManager->getElement( trtId )->strawTransform(m_trt_straw) * zaxis * m_TRTDetectorManager->getElement( trtId )->strawDirection();
    
    const Amg::Vector3D strawXAxis_NoSign = m_TRTDetectorManager->getElement( trtId )->strawTransform(m_trt_straw) * xaxis;
    const Amg::Vector3D strawYAxis_NoSign = m_TRTDetectorManager->getElement( trtId )->strawTransform(m_trt_straw) * yaxis;
    const Amg::Vector3D strawZAxis_NoSign = m_TRTDetectorManager->getElement( trtId )->strawTransform(m_trt_straw) * zaxis;
    //const HepGeom::Vector3D<double> theStrawAxis = m_TRTDetectorManager->getElement( trtId )->strawAxis(m_trt_straw);
    
    //writeTransForm(localTransform);
    writeVector("straw x-axis",strawXAxis);
    writeVector("straw y-axis",strawYAxis);
    writeVector("straw z-axis",strawZAxis);
    m_outputFile << std::endl;
    writeVector("straw x-axis (No sign)",strawXAxis_NoSign);
    writeVector("straw y-axis (No sign)",strawYAxis_NoSign);
    writeVector("straw z-axis (No sign)",strawZAxis_NoSign);
    m_outputFile << std::endl;
  }
  
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Leaving writeTRTFrames()" << endmsg;
  return;
}

void GetDetectorLocalFrames::writeVector(std::string name, const Amg::Vector3D& vector){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writeVector()" << endmsg;
  
  m_outputFile << name << " " << vector.x() << " " << vector.y() << "  " << vector.z() << std::endl;
  
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Leaving writeVector()" << endmsg;
  return;
}

void GetDetectorLocalFrames::writeTransForm(const HepGeom::Transform3D& transform){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writeTransForm()" << endmsg;
  
  m_outputFile << " Trans(x,y,z): " << transform.dx() << " " << transform.dy() << "  " << transform.dz() << std::endl;
  m_outputFile << " Rotation Matrix " << std::endl;
  m_outputFile << transform.xx() << " " << transform.xy() << "  " << transform.xz() << std::endl;
  m_outputFile << transform.yx() << " " << transform.yy() << "  " << transform.yz() << std::endl;
  m_outputFile << transform.zx() << " " << transform.zy() << "  " << transform.zz() << std::endl;

  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Leaving writeTransForm()" << endmsg;
  return;
}

