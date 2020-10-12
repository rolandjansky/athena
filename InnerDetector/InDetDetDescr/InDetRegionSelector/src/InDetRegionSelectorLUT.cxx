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
  msg(MSG::INFO) << "initialize()" << endmsg;  

  //
  // get tools
  //

  ATH_MSG_INFO( "InDetRegionSelectorLUT::initialize() " << name() << " no longer used" );
  
  return StatusCode::SUCCESS;
}
  
void InDetRegionSelectorLUT::printTable(const RegSelSiLUT* /*lut*/)
{
//NOP; WHY IS THIS HERE?
}


