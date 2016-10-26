/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ISvcLocator.h"

#include "TrigL2MuonSA/PtBarrelLUTSvc.h"
#include "PathResolver/PathResolver.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

using namespace std;

TrigL2MuonSA::PtBarrelLUTSvc::PtBarrelLUTSvc(const std::string& name,ISvcLocator* sl) :
  AthService(name,sl),
  m_ptBarrelLUT("TrigL2MuonSA::PtBarrelLUT")
{
  declareProperty( "LUTfile", m_lut_fileName="pt_barrel.lut" );
  declareProperty( "SP_LUTfile", m_lutSP_fileName="pt_barrelSP_new.lut" );
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
  ATH_MSG_DEBUG("Initializing " << name() << " - package version " << PACKAGE_VERSION);
  
  StatusCode sc;
  
  sc = AthService::initialize();
  if ( sc.isFailure() ) return sc;
  
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
  
  sc = m_ptBarrelLUT.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve " << m_ptBarrelLUT);
    return sc;
  }
  ATH_MSG_DEBUG("Retrieved service " << m_ptBarrelLUT);

  // read LUT
  sc = m_ptBarrelLUT->readLUT(lut_fileName, lutSP_fileName);
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Failed to read barrel LUT" << lut_fileName
		  << " and/or " << lutSP_fileName);
    return sc;
  }
  
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::PtBarrelLUTSvc::finalize() 
{
  return StatusCode::SUCCESS;
} 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
