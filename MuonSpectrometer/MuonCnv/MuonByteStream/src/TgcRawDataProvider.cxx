/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStream/TgcRawDataProvider.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MuonRDO/TgcRdoIdHash.h"
#include "eformat/SourceIdentifier.h"
#include <algorithm>

using eformat::helper::SourceIdentifier;

// --------------------------------------------------------------------
// Constructor

Muon::TgcRawDataProvider::TgcRawDataProvider(const std::string& name,
        ISvcLocator* pSvcLocator) :
        AthAlgorithm(name, pSvcLocator),
        m_robDataProvider ("ROBDataProviderSvc",name),
        m_rawDataTool     ("TgcRawDataProviderTool")
{
    declareProperty ("ProviderTool", m_rawDataTool);
}

// Destructor

Muon::TgcRawDataProvider::~TgcRawDataProvider()
{}

// --------------------------------------------------------------------
// Initialize

StatusCode Muon::TgcRawDataProvider::initialize()
{
    ATH_MSG_INFO( "TgcRawDataProvider::initialize"  );
    ATH_CHECK( m_robDataProvider.retrieve() );
    ATH_CHECK( m_rawDataTool.retrieve() );

    TgcRdoIdHash rdoIdHash;
    for (int i = 0; i < rdoIdHash.max(); i++)
    {
        SourceIdentifier sid((eformat::SubDetector)rdoIdHash.subDetectorId(i), rdoIdHash.rodId(i));
        m_robIds.push_back(sid.simple_code());
    }

    return StatusCode::SUCCESS;
}

StatusCode Muon::TgcRawDataProvider::finalize()
{
    return StatusCode::SUCCESS;
}
// --------------------------------------------------------------------
// Execute

StatusCode Muon::TgcRawDataProvider::execute()
{
    ATH_MSG_VERBOSE( "TgcRawDataProvider::execute"  );

    // ask ROBDataProviderSvc for the vector of ROBFragment for all TGC ROBIDs
    std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
    m_robDataProvider->getROBData(m_robIds, vecOfRobf);

    ATH_MSG_VERBOSE( "Number of ROB fragments " << vecOfRobf.size()  );

    // ask TgcRawDataProviderTool to decode it and to fill the IDC
    if (m_rawDataTool->convert(vecOfRobf).isFailure())
      ATH_MSG_ERROR( "BS conversion into RDOs failed"  );

    return StatusCode::SUCCESS;
}

