/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ISvcLocator.h"

#include "PtEndcapLUTSvc.h"
#include "PathResolver/PathResolver.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::PtEndcapLUTSvc::PtEndcapLUTSvc(const std::string& name, ISvcLocator* sl) :
  AthService(name,sl)
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::PtEndcapLUTSvc::initialize()
{   
  // implement the search of LUT trought the pathresolver Tool.
  std::string lut_fileName;
  if(m_useRun3LUT){
    lut_fileName = PathResolver::find_file(m_lut_fileNameRun3, "DATAPATH");
  } else {
    lut_fileName = PathResolver::find_file(m_lut_fileNameRun2, "DATAPATH");
  }  
  ATH_MSG_INFO(lut_fileName);
  
  if (lut_fileName.empty()) {
    ATH_MSG_ERROR("Cannot find EndcapLUT file " << m_lut_fileNameRun3);
    return StatusCode::FAILURE;
  }
  
  ATH_CHECK( m_ptEndcapLUT.retrieve() );
  ATH_MSG_DEBUG("Retrieved service " << m_ptEndcapLUT);

  // read LUT
  ATH_CHECK( m_ptEndcapLUT->readLUT(lut_fileName) );
  if(m_useRun3LUT){
    ATH_MSG_DEBUG("Read endcap LUT" << m_lut_fileNameRun3);
  } else {
    ATH_MSG_DEBUG("Read endcap LUT" << m_lut_fileNameRun2);
  }  

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

