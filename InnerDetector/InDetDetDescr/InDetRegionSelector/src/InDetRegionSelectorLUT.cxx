/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetRegionSelector/InDetRegionSelectorLUT.h"

#include "RegSelLUT/RegSelSiLUT.h"

#include <vector>
#include <string>


/////////////////////////////////////////////////////////////////////////////
//
// Computes SiDetectorElement positions and other info.

/////////////////////////////////////////////////////////////////////////////

using namespace InDetDD;

InDetRegionSelectorLUT::InDetRegionSelectorLUT(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{  
  // Get parameter values from jobOptions file
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


