/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "InDetRegionSelector/InDetRegionSelectorLUT.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "RegSelLUT/RegSelSiLUT.h" 

#include <vector>
#include <string>


/////////////////////////////////////////////////////////////////////////////
//
// Computes SiDetectorElement positions and other info.

/////////////////////////////////////////////////////////////////////////////

using namespace InDetDD;

InDetRegionSelectorLUT::InDetRegionSelectorLUT(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_lutCreatorToolPixel("InDetDD::SiRegionSelectorTable/PixelRegionSelectorTable"),
  m_lutCreatorToolSCT  ("InDetDD::SiRegionSelectorTable/SCT_RegionSelectorTable"),
  m_lutCreatorToolTRT  ("InDetDD::TRT_RegionSelectorTable/TRT_RegionSelectorTable")
{  
  // Get parameter values from jobOptions file
  declareProperty("PixelRegionLUT_CreatorTool", m_lutCreatorToolPixel);
  declareProperty("SCT_RegionLUT_CreatorTool",  m_lutCreatorToolSCT);
  declareProperty("TRT_RegionLUT_CreatorTool",  m_lutCreatorToolTRT);
  declareProperty("DoTests", m_doTests = false);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode InDetRegionSelectorLUT::initialize(){
  msg(MSG::INFO) << "initialize()" << endreq;  

  //
  // get tools
  //

  msg(MSG::INFO) << "InDetRegionSelectorLUT::initialize() " << name() << "\t" << m_lutCreatorToolPixel << endreq;
  
  // Pixel
  if (!m_lutCreatorToolPixel) {  
    msg(MSG::INFO) << "Pixel LUT Creator not configured " << m_lutCreatorToolPixel << endreq;
  } else {
    if ( m_lutCreatorToolPixel.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_lutCreatorToolPixel << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_lutCreatorToolPixel << endreq;
      // compute LUT
      const RegSelSiLUT* pixellut = m_lutCreatorToolPixel->getLUT();
      if (!pixellut) { 
		msg(MSG::ERROR) << "Pixel table missing" << endreq;
      } else if (m_doTests) {
	msg(MSG::DEBUG) << "Test Pixel" << endreq;
	printTable(pixellut);
      }
    }
  }
  // SCT
  if (!m_lutCreatorToolSCT) {  
    msg(MSG::INFO) << "SCT LUT Creator not configured " << m_lutCreatorToolSCT << endreq;
  } else {
    if ( m_lutCreatorToolSCT.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_lutCreatorToolSCT << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_lutCreatorToolSCT << endreq;
      // compute LUT
      const RegSelSiLUT* sctlut = m_lutCreatorToolSCT->getLUT();
      if (!sctlut) { 
	msg(MSG::ERROR) << "SCT table missing" << endreq;
      } else if (m_doTests) {
	msg(MSG::DEBUG) << "Test SCT" << endreq;
	printTable(sctlut);
      }
    }
  }
  // Pixel
  if (!m_lutCreatorToolTRT) {  
    msg(MSG::INFO) << "TRT LUT Creator not configured " << m_lutCreatorToolTRT << endreq;
  } else {
    if ( m_lutCreatorToolTRT.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_lutCreatorToolSCT << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_lutCreatorToolSCT << endreq;
      // compute LUT
      const RegSelSiLUT* trtlut = m_lutCreatorToolTRT->getLUT();
      if (!trtlut) { 
	msg(MSG::ERROR) << "TRT table missing" << endreq;
      } else if (m_doTests) {
	msg(MSG::DEBUG) << "Test TRT" << endreq;
	printTable(trtlut);
      }
    }
  }
  return StatusCode::SUCCESS;
}
  
void InDetRegionSelectorLUT::printTable(const RegSelSiLUT* /*lut*/)
{
//NOP; WHY IS THIS HERE?
}


