/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStream/MdtRawDataProvider.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include <algorithm>
// --------------------------------------------------------------------
// Constructor

Muon::MdtRawDataProvider::MdtRawDataProvider(const std::string& name,
                                      ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  m_log             (msgSvc(), name),
  m_rawDataTool     ("MdtRawDataProviderTool")
{
  declareProperty ("ProviderTool", m_rawDataTool);
}

// Destructor

Muon::MdtRawDataProvider::~MdtRawDataProvider(){
}

// --------------------------------------------------------------------
// Initialize

StatusCode Muon::MdtRawDataProvider::initialize() {

  m_log << MSG::INFO << "MdtRawDataProvider::initialize" << endreq;

  // Get MdtRawDataProviderTool
  if (m_rawDataTool.retrieve().isFailure()) {
    m_log << MSG::FATAL << "Failed to retrieve serive " << m_rawDataTool << endreq;
    return StatusCode::FAILURE;
  } else
    m_log << MSG::INFO << "Retrieved service " << m_rawDataTool << endreq;
  
  return StatusCode::SUCCESS;
}

StatusCode Muon::MdtRawDataProvider::finalize() {
  return StatusCode::SUCCESS;
}
// --------------------------------------------------------------------
// Execute

StatusCode Muon::MdtRawDataProvider::execute() {
  m_log.setLevel( outputLevel() );

  m_log << MSG::VERBOSE << "MdtRawDataProvider::execute" << endreq;
  
  // ask MdtRawDataProviderTool to decode it and to fill the IDC
  if (m_rawDataTool->convert().isFailure())
    m_log << MSG::ERROR << "BS conversion into RDOs failed" << endreq;

//  m_log << MSG::INFO << "Number of Collections in IDC " << m_container->numberOfCollections() << endreq;

  return StatusCode::SUCCESS;
}

