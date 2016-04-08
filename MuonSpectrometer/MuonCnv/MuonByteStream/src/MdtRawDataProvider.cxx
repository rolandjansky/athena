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
  AthAlgorithm(name, pSvcLocator),
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

  ATH_MSG_INFO( "MdtRawDataProvider::initialize" );
  ATH_CHECK( m_rawDataTool.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode Muon::MdtRawDataProvider::finalize() {
  return StatusCode::SUCCESS;
}
// --------------------------------------------------------------------
// Execute

StatusCode Muon::MdtRawDataProvider::execute() {
  ATH_MSG_VERBOSE( "MdtRawDataProvider::execute" );
  
  // ask MdtRawDataProviderTool to decode it and to fill the IDC
  if (m_rawDataTool->convert().isFailure())
    ATH_MSG_ERROR( "BS conversion into RDOs failed" );

//  ATH_MSG_INFO( "Number of Collections in IDC " << m_container->numberOfCollections() );

  return StatusCode::SUCCESS;
}

