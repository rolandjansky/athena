/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "TRT_ReadoutGeometry/TRT_Numerology.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "InDetIdentifier/TRT_ID.h"
#include "TRT_ReadoutGeometry/TRT_DetectorManager.h"

#include "InDetIdentifier/SCT_ID.h"

#include "InDetIdentifier/PixelID.h"

#include "EventPrimitives/EventPrimitives.h"

#include "StoreGate/ReadCondHandle.h"

#include <stdlib.h>
#include <string.h>
#include <vector>

/** Constructor */
GetDetectorPositions::GetDetectorPositions(std::string const&  name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_detailLevel(0),
  m_doTRT(true),
  m_outputFileName("IDgeometry.txt"),  

  /** ID Tools */
  m_PixelHelper(0),
  m_SCTHelper(0),
  m_TRTHelper(0),
  m_TRTDetectorManager(0)

{
  declareProperty("OutputTextFile",m_outputFileName);
  declareProperty("DetailLevel",   m_detailLevel);
  declareProperty("DoTRT",         m_doTRT);
}

/** initialize */
StatusCode GetDetectorPositions::initialize(){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "initialize()" << endmsg;
  
  /** Retrive TRT info */
  if(m_doTRT){
    if (detStore()->retrieve(m_TRTHelper, "TRT_ID").isFailure()) {
      msg(MSG::FATAL) << "Could not get TRT ID helper" << endmsg;
      return StatusCode::FAILURE;
    }
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "got the TRT ID" << endmsg;

    if ((detStore()->retrieve(m_TRTDetectorManager)).isFailure()) {
      if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Problem retrieving TRT_DetectorManager" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  /** Retrive SCT info */
  if (detStore()->retrieve(m_SCTHelper, "SCT_ID").isFailure()) {
    msg(MSG::FATAL) << "Could not get SCT ID helper" << endmsg;
    return StatusCode::FAILURE;
  }
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "got the SCT ID" << endmsg;
  ATH_CHECK(m_SCTDetEleCollKey.initialize());

  /** Retrive Pixel info */
  if (detStore()->retrieve(m_PixelHelper, "PixelID").isFailure()) {
    msg(MSG::FATAL) << "Could not get Pixel ID helper" << endmsg;
    return StatusCode::FAILURE;
    }
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "got the Pixel ID" << endmsg;
  ATH_CHECK(m_pixelDetEleCollKey.initialize());
  
  /** Output text File */
  m_outputFile.open((m_outputFileName).c_str());
  
  return StatusCode::SUCCESS;
}

/** execute */
StatusCode GetDetectorPositions::execute() {
  
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "execute() check global position" << endmsg;
  
  //StatusCode sc = StatusCode::SUCCESS;
  m_eventCount++;
    
    /** run only for one event */
    if (m_eventCount!=0) 
      return StatusCode::SUCCESS; 
    
    std::cout << "========================================================================================" <<std::endl;
    std::cout << "==================== Getting the Inner Detector Positions ==============================" <<std::endl;
    std::cout << "========================================================================================" <<std::endl;

    //Write pixel positions
    writePixelPositions();

    //Write SCT positions
    writeSCTPositions();

    //Write TRT positions
    if(m_doTRT)
      writeTRTPositions();

    return StatusCode::SUCCESS;
}

/** Finalize */
StatusCode GetDetectorPositions::finalize() {
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "finalize()" << endmsg;
  
  /** Close the file */
  m_outputFile.close();

  return StatusCode::SUCCESS;
}

/** Writing the Pixel Positions */
void GetDetectorPositions::writePixelPositions(){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writePixelPositions()" << endmsg;

  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> pixelDetEleHandle(m_pixelDetEleCollKey);
  const InDetDD::SiDetectorElementCollection* elements{*pixelDetEleHandle};
  if (not pixelDetEleHandle.isValid() or elements==nullptr) {
    ATH_MSG_ERROR(m_pixelDetEleCollKey.fullKey() << " is not available.");
    return;
  }
  //Loop over pixel elements
  for (const InDetDD::SiDetectorElement* si_hit: *elements) {
    Amg::Vector3D p3d = si_hit->center();

    const IdentifierHash wafer_hash = si_hit->identifyHash();
    const Identifier wafer_id = m_SCTHelper->wafer_id(wafer_hash);
    
    int   pix_barrel_ec = m_PixelHelper->barrel_ec(wafer_id);
    int   pix_layer_disk= m_PixelHelper->layer_disk(wafer_id);
    int   pix_phi_module= m_PixelHelper->phi_module(wafer_id);
    int   pix_eta_module = m_PixelHelper->eta_module(wafer_id);
    int   nPixPhi      = m_PixelHelper->phi_index_max(wafer_id)+1;
    int   nPixEta      = m_PixelHelper->eta_index_max(wafer_id)+1;
    float pix_x = p3d.x();
    float pix_y = p3d.y();
    float pix_z = p3d.z();
    
    m_outputFile << 1 << " "
	       << pix_barrel_ec << " " 
	       << pix_layer_disk << " " 
	       << pix_phi_module << " "
	       << pix_eta_module << " "
	       << pix_x << " "
	       << pix_y << " " 
	       << pix_z << " " 
	       << nPixPhi << " " 
	       << nPixEta << " " 
	       << std::endl;
  }
  
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Leaving writePixelPositions()" << endmsg;
  return;
}

/** Writing the SCT Positions */
void GetDetectorPositions::writeSCTPositions(){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writeSCTPositions()" << endmsg;
  
  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEleHandle(m_SCTDetEleCollKey);
  const InDetDD::SiDetectorElementCollection* elements{*sctDetEleHandle};
  if (not sctDetEleHandle.isValid() or elements==nullptr) {
    ATH_MSG_ERROR(m_SCTDetEleCollKey.fullKey() << " is not available.");
    return;
  }
  //Loop over SCT elements
  for (const InDetDD::SiDetectorElement* si_hit: *elements) {
    Amg::Vector3D p3d = si_hit->center();

    const IdentifierHash wafer_hash = si_hit->identifyHash();
    const Identifier wafer_id = m_SCTHelper->wafer_id(wafer_hash);
    
    int sct_barrel_ec = m_SCTHelper->barrel_ec(wafer_id);
    int sct_layer_disk= m_SCTHelper->layer_disk(wafer_id);
    int sct_phi_module= m_SCTHelper->phi_module(wafer_id);
    int sct_eta_module = m_SCTHelper->eta_module(wafer_id);
    int nStrips      = m_SCTHelper->strip_max(wafer_id)+1;
    float sct_x = p3d.x();
    float sct_y = p3d.y();
    float sct_z = p3d.z();
    
    m_outputFile << 2 << " "
	       << sct_barrel_ec << " "
	       << sct_layer_disk << " "
	       << sct_phi_module << " "
	       << sct_eta_module << " " 
	       << sct_x << " "
	       << sct_y << " "
	       << sct_z << " " 
	       << nStrips << " " 
	       << std::endl;
  }
  
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Leaving writeSCTPositions()" << endmsg;
  return;
}

/** Writing the TRT positions (depends on the level of detail) */
void GetDetectorPositions::writeTRTPositions(){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writeTRTPositions()" << endmsg;

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
    
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writeTRTPositions()" << endmsg;
  return;  
}

/** Writing the Simple (not all straws) TRT Positions */
void GetDetectorPositions::writeSimpleTRTPositions(){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writeSimpleTRTPositions()" << endmsg;

  //Loop over TRT elements
  std::vector<Identifier>::const_iterator trtStrawLayIt = m_TRTHelper->straw_layer_begin();
  std::vector<Identifier>::const_iterator trtStrawLayItE = m_TRTHelper->straw_layer_end();

  for(; trtStrawLayIt != trtStrawLayItE; trtStrawLayIt++  ) {
    writeTRTPositions(*trtStrawLayIt);
  }

  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writeTRTPositions()" << endmsg;
  return;  
}

/** Writing the Detailed (all straws) TRT Positions */
void GetDetectorPositions::writeDetailedTRTPositions(bool doDoubleSampling){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writeDetailedTRTPositions()" << endmsg;
  
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
  
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Leaving writeDetailedTRTPositions()" << endmsg;
  return;
}

/** Write the TRT Positions */
void GetDetectorPositions::writeTRTPositions(Identifier const& trtId){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writeTRTPositions()" << endmsg;

  const Amg::Vector3D  &p3d = m_TRTDetectorManager->getElement( trtId )->center(trtId);
  
  
  int trt_barrel_ec = m_TRTHelper->barrel_ec(trtId);
  int trt_layer_or_wheel = m_TRTHelper->layer_or_wheel(trtId);
  int trt_phi_module = m_TRTHelper->phi_module(trtId);
  int trt_straw_layer = m_TRTHelper->straw_layer(trtId);
  int trt_straw = m_TRTHelper->straw(trtId);
  float trt_x = p3d.x();
  float trt_y = p3d.y();
  float trt_z = p3d.z();
  
  m_outputFile << 3 << " "
	     << trt_barrel_ec << " "
	     << trt_layer_or_wheel << " "
	     << trt_phi_module << " "
	     << trt_straw_layer << " ";
  
  if(m_detailLevel)	     
    m_outputFile << trt_straw << " ";
  
  m_outputFile << trt_x << " "
	     << trt_y << " "
	     << trt_z << " "
	     << std::endl;
  
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Leaving writeTRTPositions()" << endmsg;
  return;
}



/** Write the TRT Positions, sampling each straw twice
    Allows the wire-by-wire alignment to be debugged
 */
void GetDetectorPositions::writeTwoTRTPositions(Identifier const& trtId){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writeTwoTRTPositions()" << endmsg;

  int trt_barrel_ec = m_TRTHelper->barrel_ec(trtId);
  int trt_layer_or_wheel = m_TRTHelper->layer_or_wheel(trtId);
  int trt_phi_module = m_TRTHelper->phi_module(trtId);
  int trt_straw_layer = m_TRTHelper->straw_layer(trtId);
  int trt_straw = m_TRTHelper->straw(trtId);
 
  // We sample the straw position at two places
  Amg::Vector3D posZSample(0,0,100);
  Amg::Vector3D firstSampling = m_TRTDetectorManager->getElement( trtId )->strawTransform(trt_straw) * posZSample;
  float first_X = firstSampling.x();
  float first_Y = firstSampling.y();
  float first_Z = firstSampling.z();

  Amg::Vector3D negZSample(0,0,-100);
  Amg::Vector3D secondSampling = m_TRTDetectorManager->getElement( trtId )->strawTransform(trt_straw) * negZSample;
  float second_X = secondSampling.x();
  float second_Y = secondSampling.y();
  float second_Z = secondSampling.z();
  
  m_outputFile << 3 << " "
	     << trt_barrel_ec << " "
	     << trt_layer_or_wheel << " "
	     << trt_phi_module << " "
	     << trt_straw_layer << " "
	     << trt_straw << " "
  	     << first_X << " "
	     << first_Y << " "
	     << first_Z << " "
	     << std::endl;

  m_outputFile << 3 << " "
	     << trt_barrel_ec << " "
	     << trt_layer_or_wheel << " "
	     << trt_phi_module << " "
	     << trt_straw_layer << " "
	     << trt_straw << " "
  	     << second_X << " "
	     << second_Y << " "
	     << second_Z << " "
	     << std::endl;

  
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Leaving writeTRTPositions()" << endmsg;
  return;
}
