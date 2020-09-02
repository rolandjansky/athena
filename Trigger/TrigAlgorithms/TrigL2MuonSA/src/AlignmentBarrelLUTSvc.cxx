/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ISvcLocator.h"

#include "AlignmentBarrelLUTSvc.h"
#include "PathResolver/PathResolver.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

TrigL2MuonSA::AlignmentBarrelLUTSvc::AlignmentBarrelLUTSvc(const std::string& name,ISvcLocator* sl) :
  AthService(name,sl)
{
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

  // implement the search of LUT trought the pathresolver Tool.
  std::string lut_fileName = PathResolver::find_file (m_lut_fileName, "DATAPATH");
  ATH_MSG_INFO(lut_fileName);

  if (lut_fileName.empty()) {
    ATH_MSG_ERROR("Cannot find EndcapLUT file " << lut_fileName);
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_alignmentBarrelLUT.retrieve() );

  // read LUT
  ATH_CHECK( m_alignmentBarrelLUT->readLUT(lut_fileName) );

  return StatusCode::SUCCESS;
}
