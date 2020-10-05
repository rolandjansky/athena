/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ISvcLocator.h"

#include "PtBarrelLUTSvc.h"
#include "PathResolver/PathResolver.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

TrigL2MuonSA::PtBarrelLUTSvc::PtBarrelLUTSvc(const std::string& name,ISvcLocator* sl) :
  AthService(name,sl)
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::PtBarrelLUTSvc::queryInterface(const InterfaceID& riid, void** ppvIF) 
{ 
  if (TrigL2MuonSA::PtBarrelLUTSvc::interfaceID().versionMatch(riid)) {
    *ppvIF = (PtBarrelLUTSvc*)this;
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG(name() << " cannot find the interface! Query the interface of the base class.");
    return AthService::queryInterface(riid, ppvIF);
  }
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::PtBarrelLUTSvc::initialize()
{
  
  // implement the search of LUT trought the pathresolver Tool.
  std::string lut_fileName = PathResolver::find_file (m_lut_fileName, "DATAPATH");
  ATH_MSG_INFO(lut_fileName);
  
  if (lut_fileName.empty()) {
    ATH_MSG_ERROR("Cannot find barrel LUT file " << lut_fileName);
    return StatusCode::FAILURE;
  }
  
  // implement the search of SP LUT trought the pathresolver Tool.
  std::string lutSP_fileName = PathResolver::find_file (m_lutSP_fileName, "DATAPATH");
  ATH_MSG_INFO(lutSP_fileName);
  
  if (lutSP_fileName.empty()) {
    ATH_MSG_ERROR("Cannot find barrel SP LUT file " << lutSP_fileName);
    return StatusCode::FAILURE;
  }
  
  ATH_CHECK( m_ptBarrelLUT.retrieve() );
  ATH_MSG_DEBUG("Retrieved service " << m_ptBarrelLUT);

  // read LUT
  ATH_CHECK( m_ptBarrelLUT->readLUT(lut_fileName, lutSP_fileName) );
  
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
