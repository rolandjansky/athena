/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ISvcLocator.h"

#include "TrigL2MuonSA/PtEndcapLUTSvc.h"
#include "PathResolver/PathResolver.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::PtEndcapLUTSvc::PtEndcapLUTSvc(const std::string& name, ISvcLocator* sl) :
  AthService(name,sl),
  m_ptEndcapLUT("TrigL2MuonSA::PtEndcapLUT")
{
  declareProperty("FileNameRun2", m_lut_fileName="pt_endcap_run2.lut");
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::PtEndcapLUTSvc::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  if (TrigL2MuonSA::PtEndcapLUTSvc::interfaceID().versionMatch(riid)) {
    *ppvIF = (PtEndcapLUTSvc*)this;
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG(name() << " cannot find the interface! Query the interface of the base class.");
    return AthService::queryInterface(riid, ppvIF);
  }
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::PtEndcapLUTSvc::initialize()
{   
  StatusCode sc;
  
  sc = AthService::initialize();
  if ( sc.isFailure() ) return sc;
 
  // implement the search of LUT trought the pathresolver Tool.
  std::string lut_fileName = PathResolver::find_file(m_lut_fileName, "DATAPATH");
  ATH_MSG_INFO(lut_fileName);
  
  if (lut_fileName.empty()) {
    ATH_MSG_ERROR("Cannot find EndcapLUT file " << m_lut_fileName);
    return StatusCode::FAILURE;
  }
  
  sc = m_ptEndcapLUT.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve " << m_ptEndcapLUT);
    return sc;
  }
  ATH_MSG_DEBUG("Retrieved service " << m_ptEndcapLUT);

  // read LUT
  sc = m_ptEndcapLUT->readLUT(lut_fileName);
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Failed to read endcap LUT" << m_lut_fileName);
    return sc;
  }

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::PtEndcapLUTSvc::finalize()
{
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
