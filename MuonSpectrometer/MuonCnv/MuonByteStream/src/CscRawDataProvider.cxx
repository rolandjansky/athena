/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStream/CscRawDataProvider.h"
#include "CSCcabling/CSCcablingSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include <algorithm>
// --------------------------------------------------------------------
// Constructor

Muon::CscRawDataProvider::CscRawDataProvider(const std::string& name,
                                      ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_rawDataTool     ("CscRawDataProviderTool")
{
  declareProperty ("ProviderTool", m_rawDataTool);
}

// Destructor

Muon::CscRawDataProvider::~CscRawDataProvider(){
}

// --------------------------------------------------------------------
// Initialize
StatusCode Muon::CscRawDataProvider::initialize() {
  ATH_MSG_VERBOSE(" in initialize()");

  // Get CscRawDataProviderTool
  if (m_rawDataTool.retrieve().isFailure()) {
    ATH_MSG_FATAL( "Failed to retrieve serive " << m_rawDataTool );
    return StatusCode::FAILURE;
  } else
    ATH_MSG_VERBOSE("Retrieved service " << m_rawDataTool );
  
  return StatusCode::SUCCESS;
}

StatusCode Muon::CscRawDataProvider::finalize() {
  return StatusCode::SUCCESS;
}
// --------------------------------------------------------------------
// Execute

StatusCode Muon::CscRawDataProvider::execute() {
  ATH_MSG_VERBOSE( "CscRawDataProvider::execute" );

  // ask CscRawDataProviderTool to decode entire event and to fill the IDC
  if (m_rawDataTool->convert().isFailure())
    ATH_MSG_ERROR ( "BS conversion into RDOs failed" );

  return StatusCode::SUCCESS;
}

