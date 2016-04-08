/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStream/RpcRawDataProvider.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include <algorithm>
// --------------------------------------------------------------------
// Constructor

Muon::RpcRawDataProvider::RpcRawDataProvider(const std::string& name,
                                      ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_rawDataTool     ("RpcRawDataProviderTool")
  //m_cablingSvc       ("IRPCcablingSvc", name) 
{
  declareProperty ("ProviderTool", m_rawDataTool);
}

// Destructor

Muon::RpcRawDataProvider::~RpcRawDataProvider(){
}

// --------------------------------------------------------------------
// Initialize

StatusCode Muon::RpcRawDataProvider::initialize() {
  ATH_MSG_INFO( "RpcRawDataProvider::initialize"  );
  ATH_CHECK( m_rawDataTool.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode Muon::RpcRawDataProvider::finalize() {
  return StatusCode::SUCCESS;
}
// --------------------------------------------------------------------
// Execute

StatusCode Muon::RpcRawDataProvider::execute() {
  ATH_MSG_VERBOSE( "RpcRawDataProvider::execute"  );

  // ask RpcRawDataProviderTool to decode it and to fill the IDC
  if (m_rawDataTool->convert().isFailure())
    ATH_MSG_ERROR( "BS conversion into RDOs failed"  );

  return StatusCode::SUCCESS;
}

