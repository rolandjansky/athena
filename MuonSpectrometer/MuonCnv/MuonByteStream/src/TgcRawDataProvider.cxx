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
        Algorithm(name, pSvcLocator),
        m_log             (msgSvc(), name),
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
    m_log << MSG::INFO << "TgcRawDataProvider::initialize" << endreq;

    // Get ROBDataProviderSvc
    if (m_robDataProvider.retrieve().isFailure())
    {
        m_log << MSG::FATAL << "Failed to retrieve serive " << m_robDataProvider << endreq;
        return StatusCode::FAILURE;
    }
    else
        m_log << MSG::INFO << "Retrieved service " << m_robDataProvider << endreq;

    // Get TgcRawDataProviderTool
    if (m_rawDataTool.retrieve().isFailure())
    {
        m_log << MSG::FATAL << "Failed to retrieve serive " << m_rawDataTool << endreq;
        return StatusCode::FAILURE;
    }
    else
        m_log << MSG::INFO << "Retrieved service " << m_rawDataTool << endreq;

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
    m_log.setLevel( outputLevel() );
    m_log << MSG::VERBOSE << "TgcRawDataProvider::execute" << endreq;

    // ask ROBDataProviderSvc for the vector of ROBFragment for all TGC ROBIDs
    std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
    m_robDataProvider->getROBData(m_robIds, vecOfRobf);

    m_log << MSG::VERBOSE << "Number of ROB fragments " << vecOfRobf.size() << endreq;

    // ask TgcRawDataProviderTool to decode it and to fill the IDC
    if (m_rawDataTool->convert(vecOfRobf).isFailure())
        m_log << MSG::ERROR << "BS conversion into RDOs failed" << endreq;

    return StatusCode::SUCCESS;
}

