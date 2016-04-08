/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
    Authors: John Alison <johnda@hep.upenn.edu>
    
    Date: 22 Aug 2008
    
    Description: This algorithm loops over the Inner Detector elements 
    and prints thier global positions to a text file

*/

#include "InDetSimpleVisual/GetDetectorPositions.h"

#include "GaudiKernel/MsgStream.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetReadoutGeometry/TRT_Numerology.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "EventPrimitives/EventPrimitives.h"

#include <stdlib.h>
#include <string.h>
#include <vector>

/** Constructor */
GetDetectorPositions::GetDetectorPositions(std::string const&  name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_detailLevel(0),
  m_outputFileName("IDgeometry.txt"),  
  
  /** Pixel Variables */
  m_pix_barrel_ec(0),
  m_pix_layer_disk(0),
  m_pix_phi_module(0),
  m_pix_eta_module(0),
  m_pix_x(0),
  m_pix_y(0),
  m_pix_z(0),
  
  /** SCT variables */  
  m_sct_barrel_ec(0),
  m_sct_layer_disk(0),
  m_sct_phi_module(0),
  m_sct_eta_module(0),
  m_sct_x(0),
  m_sct_y(0),
  m_sct_z(0),

  /** TRT variables */    
  m_trt_barrel_ec(0),
  m_trt_layer_or_wheel(0),
  m_trt_phi_module(0),
  m_trt_straw_layer(0),
  m_trt_straw(0),
  m_trt_x(0),
  m_trt_y(0),
  m_trt_z(0),

  /** ID Tools */
  m_PixelHelper(0),
  m_pixelDetectorManager(0),
  m_SCTHelper(0),
  m_SCTDetectorManager(0),
  m_TRTHelper(0),
  m_TRTDetectorManager(0)

{
  declareProperty("OutputTextFile",m_outputFileName);
  declareProperty("DetailLevel",m_detailLevel);
}

/** initialize */
StatusCode GetDetectorPositions::initialize(){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "initialize()" << endreq;
  
  /** Retrive TRT info */
  if (detStore()->retrieve(m_TRTHelper, "TRT_ID").isFailure()) {
    msg(MSG::FATAL) << "Could not get TRT ID helper" << endreq;
    return StatusCode::FAILURE;
  }
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "got the TRT ID" << endreq;

  if ((detStore()->retrieve(m_TRTDetectorManager)).isFailure()) {
    if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Problem retrieving TRT_DetectorManager" << endreq;
    return StatusCode::FAILURE;
  }
  
  /** Retrive SCT info */
  if (detStore()->retrieve(m_SCTHelper, "SCT_ID").isFailure()) {
    msg(MSG::FATAL) << "Could not get SCT ID helper" << endreq;
    return StatusCode::FAILURE;
  }
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "got the SCT ID" << endreq;

  if ((detStore()->retrieve(m_SCTDetectorManager)).isFailure()) {
    if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Problem retrieving SCT_DetectorManager" << endreq;
    return StatusCode::FAILURE;
  }
  
  /** Retrive Pixel info */
  if (detStore()->retrieve(m_PixelHelper, "PixelID").isFailure()) {
    msg(MSG::FATAL) << "Could not get Pixel ID helper" << endreq;
    return StatusCode::FAILURE;
    }
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "got the Pixel ID" << endreq;
  
  if ((detStore()->retrieve(m_pixelDetectorManager)).isFailure()) {
    if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Problem retrieving PixelDetectorManager" << endreq;
    return StatusCode::FAILURE;
  }
  
  /** Output text File */
  outputFile.open((m_outputFileName).c_str());
  
  return StatusCode::SUCCESS;
}

/** execute */
StatusCode GetDetectorPositions::execute() {
  
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "execute() check global position" << endreq;
  
  //StatusCode sc = StatusCode::SUCCESS;
    static int eventCount(-1); eventCount++;
    
    /** run only for one event */
    if (eventCount!=0) 
      return StatusCode::SUCCESS; 
    
    std::cout << "========================================================================================" <<std::endl;
    std::cout << "==================== Getting the Inner Detector Positions ==============================" <<std::endl;
    std::cout << "========================================================================================" <<std::endl;

    //Write pixel positions
    writePixelPositions();

    //Write SCT positions
    writeSCTPositions();

    //Write SCT positions
    writeTRTPositions();

    return StatusCode::SUCCESS;
}

/** Finalize */
StatusCode GetDetectorPositions::finalize() {
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "finalize()" << endreq;
  
  /** Close the file */
  outputFile.close();

  return StatusCode::SUCCESS;
}

/** Writing the Pixel Positions */
void GetDetectorPositions::writePixelPositions(){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writePixelPositions()" << endreq;

  //Loop over pixel elements
  std::vector<Identifier>::const_iterator pixIt = m_PixelHelper->wafer_begin();
  std::vector<Identifier>::const_iterator pixItE = m_PixelHelper-> wafer_end();
  for(; pixIt != pixItE; pixIt++  ) {
    
    InDetDD::SiDetectorElement* si_hit = m_pixelDetectorManager->getDetectorElement( *pixIt );
    Amg::Vector3D p3d = si_hit->center();
    
    m_pix_barrel_ec = m_PixelHelper->barrel_ec(*pixIt);
    m_pix_layer_disk= m_PixelHelper->layer_disk(*pixIt);
    m_pix_phi_module= m_PixelHelper->phi_module(*pixIt);
    m_pix_eta_module = m_PixelHelper->eta_module(*pixIt);
    m_pix_x = p3d.x();
    m_pix_y = p3d.y();
    m_pix_z = p3d.z();
    
    outputFile << 1 << " "
	       << m_pix_barrel_ec << " " 
	       << m_pix_layer_disk << " " 
	       << m_pix_phi_module << " "
	       << m_pix_eta_module << " "
	       << m_pix_x << " "
	       << m_pix_y << " " 
	       << m_pix_z << " " 
	       << std::endl;
  }
  
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Leaving writePixelPositions()" << endreq;
  return;
}

/** Writing the SCT Positions */
void GetDetectorPositions::writeSCTPositions(){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writeSCTPositions()" << endreq;
  
  //Loop over SCT elements
  std::vector<Identifier>::const_iterator sctIt = m_SCTHelper->wafer_begin();
  std::vector<Identifier>::const_iterator sctItE = m_SCTHelper->wafer_end();
  for(; sctIt != sctItE; sctIt++  ) {
    
    InDetDD::SiDetectorElement* si_hit = m_SCTDetectorManager->getDetectorElement( *sctIt );
    Amg::Vector3D p3d = si_hit->center();
    
    m_sct_barrel_ec = m_SCTHelper->barrel_ec(*sctIt);
    m_sct_layer_disk= m_SCTHelper->layer_disk(*sctIt);
    m_sct_phi_module= m_SCTHelper->phi_module(*sctIt);
    m_sct_eta_module = m_SCTHelper->eta_module(*sctIt);
    m_sct_x = p3d.x();
    m_sct_y = p3d.y();
    m_sct_z = p3d.z();
    
    outputFile << 2 << " "
	       << m_sct_barrel_ec << " "
	       << m_sct_layer_disk << " "
	       << m_sct_phi_module << " "
	       << m_sct_eta_module << " " 
	       << m_sct_x << " "
	       << m_sct_y << " "
	       << m_sct_z << " " 
	       << std::endl;
  }
  
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Leaving writeSCTPositions()" << endreq;
  return;
}

/** Writing the TRT positions (depends on the level of detail) */
void GetDetectorPositions::writeTRTPositions(){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writeTRTPositions()" << endreq;

  //Write TRT positions
  switch(m_detailLevel) {
    
  case 0 : 
    // Only sample each 32nd in phi
    writeSimpleTRTPositions();
    break;

  case 1 : 
    // Sample each straw
    writeDetailedTRTPositions();
    break;

  case 2 :
    // Sample each straw twice 
    writeDetailedTRTPositions(true);
    break;
  }
    
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writeTRTPositions()" << endreq;
  return;  
}

/** Writing the Simple (not all straws) TRT Positions */
void GetDetectorPositions::writeSimpleTRTPositions(){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writeSimpleTRTPositions()" << endreq;

  //Loop over TRT elements
  std::vector<Identifier>::const_iterator trtStrawLayIt = m_TRTHelper->straw_layer_begin();
  std::vector<Identifier>::const_iterator trtStrawLayItE = m_TRTHelper->straw_layer_end();

  for(; trtStrawLayIt != trtStrawLayItE; trtStrawLayIt++  ) {
    writeTRTPositions(*trtStrawLayIt);
  }

  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writeTRTPositions()" << endreq;
  return;  
}

/** Writing the Detailed (all straws) TRT Positions */
void GetDetectorPositions::writeDetailedTRTPositions(bool doDoubleSampling){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writeDetailedTRTPositions()" << endreq;
  
  TRT_ID::const_expanded_id_iterator trtStrawIt = m_TRTHelper->straw_begin();
  TRT_ID::const_expanded_id_iterator trtStrawItE = m_TRTHelper->straw_end();
  
  for (; trtStrawIt != trtStrawItE; ++trtStrawIt) {
    const ExpandedIdentifier& expId = (*trtStrawIt);
    
    Identifier trtId = m_TRTHelper->straw_id(expId[2], 
					     expId[3], 
					     expId[4],
					     expId[5], 
					     expId[6]);
    
    if(doDoubleSampling)
      writeTwoTRTPositions(trtId);  
    else
      writeTRTPositions(trtId);
  }
  
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Leaving writeDetailedTRTPositions()" << endreq;
  return;
}

/** Write the TRT Positions */
void GetDetectorPositions::writeTRTPositions(Identifier const& trtId){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writeTRTPositions()" << endreq;

  const Amg::Vector3D  &p3d = m_TRTDetectorManager->getElement( trtId )->center(trtId);
  
  
    m_trt_barrel_ec = m_TRTHelper->barrel_ec(trtId);
  m_trt_layer_or_wheel = m_TRTHelper->layer_or_wheel(trtId);
  m_trt_phi_module = m_TRTHelper->phi_module(trtId);
  m_trt_straw_layer = m_TRTHelper->straw_layer(trtId);
  m_trt_straw = m_TRTHelper->straw(trtId);
  m_trt_x = p3d.x();
  m_trt_y = p3d.y();
  m_trt_z = p3d.z();
  
  outputFile << 3 << " "
	     << m_trt_barrel_ec << " "
	     << m_trt_layer_or_wheel << " "
	     << m_trt_phi_module << " "
	     << m_trt_straw_layer << " ";
  
  if(m_detailLevel)	     
    outputFile << m_trt_straw << " ";
  
  outputFile << m_trt_x << " "
	     << m_trt_y << " "
	     << m_trt_z << " "
	     << std::endl;
  
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Leaving writeTRTPositions()" << endreq;
  return;
}



/** Write the TRT Positions, sampling each straw twice
    Allows the wire-by-wire alignment to be debugged
 */
void GetDetectorPositions::writeTwoTRTPositions(Identifier const& trtId){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writeTwoTRTPositions()" << endreq;

  const Amg::Vector3D &p3d = m_TRTDetectorManager->getElement( trtId )->center(trtId);
  
  m_trt_barrel_ec = m_TRTHelper->barrel_ec(trtId);
  m_trt_layer_or_wheel = m_TRTHelper->layer_or_wheel(trtId);
  m_trt_phi_module = m_TRTHelper->phi_module(trtId);
  m_trt_straw_layer = m_TRTHelper->straw_layer(trtId);
  m_trt_straw = m_TRTHelper->straw(trtId);
  m_trt_x = p3d.x();
  m_trt_y = p3d.y();
  m_trt_z = p3d.z();
 
  // We sample the straw position at two places
  Amg::Vector3D posZSample(0,0,100);
  Amg::Vector3D firstSampling = m_TRTDetectorManager->getElement( trtId )->strawTransform(m_trt_straw) * posZSample;
  float m_first_X = firstSampling.x();
  float m_first_Y = firstSampling.y();
  float m_first_Z = firstSampling.z();

  Amg::Vector3D negZSample(0,0,-100);
  Amg::Vector3D secondSampling = m_TRTDetectorManager->getElement( trtId )->strawTransform(m_trt_straw) * negZSample;
  float m_second_X = secondSampling.x();
  float m_second_Y = secondSampling.y();
  float m_second_Z = secondSampling.z();
  
  outputFile << 3 << " "
	     << m_trt_barrel_ec << " "
	     << m_trt_layer_or_wheel << " "
	     << m_trt_phi_module << " "
	     << m_trt_straw_layer << " "
	     << m_trt_straw << " "
  	     << m_first_X << " "
	     << m_first_Y << " "
	     << m_first_Z << " "
	     << std::endl;

  outputFile << 3 << " "
	     << m_trt_barrel_ec << " "
	     << m_trt_layer_or_wheel << " "
	     << m_trt_phi_module << " "
	     << m_trt_straw_layer << " "
	     << m_trt_straw << " "
  	     << m_second_X << " "
	     << m_second_Y << " "
	     << m_second_Z << " "
	     << std::endl;

  
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Leaving writeTRTPositions()" << endreq;
  return;
}
