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
  Algorithm(name, pSvcLocator),
  m_log             (msgSvc(), name),
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
  m_log.setLevel( outputLevel() );

  m_log << MSG::INFO << "RpcRawDataProvider::initialize" << endreq;

  // Get RpcRawDataProviderTool
  if (m_rawDataTool.retrieve().isFailure()) {
    m_log << MSG::FATAL << "Failed to retrieve serive " << m_rawDataTool << endreq;
    return StatusCode::FAILURE;
  } else
    m_log << MSG::INFO << "Retrieved service " << m_rawDataTool << endreq;
  
  return StatusCode::SUCCESS;
}

StatusCode Muon::RpcRawDataProvider::finalize() {
  return StatusCode::SUCCESS;
}
// --------------------------------------------------------------------
// Execute

StatusCode Muon::RpcRawDataProvider::execute() {
  m_log.setLevel( outputLevel() );
    
  m_log << MSG::VERBOSE << "RpcRawDataProvider::execute" << endreq;

  // ask RpcRawDataProviderTool to decode it and to fill the IDC
  if (m_rawDataTool->convert().isFailure())
    m_log << MSG::ERROR << "BS conversion into RDOs failed" << endreq;

  return StatusCode::SUCCESS;
}

