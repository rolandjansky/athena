/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ISvcLocator.h"

#include "TrigL2MuonSA/AlignmentBarrelLUTSvc.h"
#include "PathResolver/PathResolver.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

using namespace std;

TrigL2MuonSA::AlignmentBarrelLUTSvc::AlignmentBarrelLUTSvc(const std::string& name,ISvcLocator* sl) :
  AthService(name,sl),
  m_alignmentBarrelLUT("TrigL2MuonSA::AlignmentBarrelLUT")
{
  declareProperty( "LUTfile", m_lut_fileName="dZ_barrel.lut" );
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::AlignmentBarrelLUTSvc::queryInterface(const InterfaceID& riid, void** ppvIF) 
{ 
  if (TrigL2MuonSA::AlignmentBarrelLUTSvc::interfaceID().versionMatch(riid)) {
    *ppvIF = (AlignmentBarrelLUTSvc*)this;
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG(name() << " cannot find the interface! Query the interface of the base class.");
    return AthService::queryInterface(riid, ppvIF);
  }
} 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::AlignmentBarrelLUTSvc::initialize()
{
  ATH_MSG_DEBUG("Initializing " << name() << " - package version " << PACKAGE_VERSION);
  
  StatusCode sc;
  
  sc = AthService::initialize();
  if ( sc.isFailure() ) return sc;
  
  // implement the search of LUT trought the pathresolver Tool.
  std::string lut_fileName = PathResolver::find_file (m_lut_fileName, "DATAPATH");
  ATH_MSG_INFO(lut_fileName);
  
  if (lut_fileName.empty()) {
    ATH_MSG_ERROR("Cannot find EndcapLUT file " << lut_fileName);
    return StatusCode::FAILURE;
  }
  
  sc = m_alignmentBarrelLUT.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve " << m_alignmentBarrelLUT);
    return sc;
  }
  ATH_MSG_DEBUG("Retrieved service " << m_alignmentBarrelLUT);

  // read LUT
  sc = m_alignmentBarrelLUT->readLUT(lut_fileName);
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Failed to read barrel alignment LUT" << lut_fileName);
    return sc;
  }
  
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::AlignmentBarrelLUTSvc::finalize() 
{
  return StatusCode::SUCCESS;
} 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
