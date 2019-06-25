/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CSC_RawDataProviderTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "CSC_RawDataProviderTool.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "ByteStreamData/ROBData.h"

#include "MuonIdHelpers/CscIdHelper.h"

#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamData/RawEvent.h"

#include "GaudiKernel/IJobOptionsSvc.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;


//================ Constructor =================================================

Muon::CSC_RawDataProviderTool::CSC_RawDataProviderTool(const std::string& t,
                                                       const std::string& n,
                                                       const IInterface*  p) :
  AthAlgTool(t, n, p),
  m_decoder("Muon::CscROD_Decoder/CscROD_Decoder", this),
  m_muonMgr(0),
  m_robDataProvider("ROBDataProviderSvc",n),
  m_cabling("CSCcablingSvc", n)
{
  declareInterface<IMuonRawDataProviderTool>(this);
  declareProperty("Decoder",     m_decoder);
  declareProperty ("CscContainerCacheKey", m_rdoContainerCacheKey, "Optional external cache for the CSC container");

}

//================ Destructor =================================================

Muon::CSC_RawDataProviderTool::~CSC_RawDataProviderTool()
{}

//================ Initialisation =================================================

StatusCode Muon::CSC_RawDataProviderTool::initialize()
{
  
  ATH_CHECK( detStore()->retrieve( m_muonMgr ) );
  ATH_CHECK( m_cabling.retrieve() );
  ATH_CHECK( m_robDataProvider.retrieve() );
  ATH_MSG_INFO ( "Retrieved service " << m_robDataProvider );
  
  
  const CscIdHelper* idHelper = m_muonMgr->cscIdHelper();
  m_hid2re.set( &(*m_cabling), idHelper );

  // Initialise the container cache if available  
  ATH_CHECK( m_rdoContainerCacheKey.initialize( !m_rdoContainerCacheKey.key().empty() ) );

  // Retrieve decoder
  if (m_decoder.retrieve().isFailure()) {
    ATH_MSG_FATAL ( "Failed to retrieve tool " << m_decoder );
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO ( "Retrieved tool " << m_decoder );

  ATH_MSG_INFO ( "The Muon Geometry version is " << m_muonMgr->geometryVersion() );

  ATH_CHECK( m_containerKey.initialize() );
  ATH_CHECK( m_eventInfoKey.initialize() );

  ATH_MSG_INFO ( "initialize() successful in " << name() );
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Muon::CSC_RawDataProviderTool::finalize()
{
  return StatusCode::SUCCESS;
}

//============================================================================================

// new one
StatusCode Muon::CSC_RawDataProviderTool::convert(const std::vector<IdentifierHash>& rdoIdhVect){

  const CscIdHelper* idHelper = m_muonMgr->cscIdHelper();
  IdContext cscContext = idHelper->module_context();

  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
  std::vector< uint32_t > robIds;
  
  for (unsigned int i=0; i<rdoIdhVect.size(); ++i) {
    uint32_t rob_id = 0xffff;
    m_cabling->hash2RobFull(rdoIdhVect[i],rob_id);
    robIds.push_back(rob_id);
  }
  m_robDataProvider->getROBData(robIds, vecOfRobf);
  ATH_MSG_VERBOSE ( "Number of ROB fragments " << vecOfRobf.size() );
  return convert(vecOfRobf, rdoIdhVect);
}

StatusCode Muon::CSC_RawDataProviderTool::convert(const EventContext& ctx) const {
  
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
  const std::vector< uint32_t >& robIds = m_hid2re.allRobIds();
  ATH_MSG_VERBOSE ( "Number of ROB ids " << robIds.size() );
  // ask ROBDataProviderSvc for the vector of ROBFragment for all MDT ROBIDs
  m_robDataProvider->getROBData( robIds, vecOfRobf);
  ATH_MSG_VERBOSE ( "Number of ROB fragments " << vecOfRobf.size() );

  return convert(vecOfRobf, ctx);
}


StatusCode Muon::CSC_RawDataProviderTool::convert(const ROBFragmentList& vecRobs,
                                                  const std::vector<IdentifierHash>& /* collections */){
  const CSC_RawDataProviderTool* cthis = this;
  return cthis->convert (vecRobs, Gaudi::Hive::currentContext());
}

StatusCode
Muon::CSC_RawDataProviderTool::convert(const ROBFragmentList& vecRobs,
                                       const EventContext& ctx) const
{

  SG::WriteHandle<CscRawDataContainer> rdoContainerHandle(m_containerKey, ctx);
  if (rdoContainerHandle.isPresent())
    return StatusCode::SUCCESS;

  // Split the methods to have one where we use the cache and one where we just setup the container
  const bool externalCacheRDO = !m_rdoContainerCacheKey.key().empty();
  if(!externalCacheRDO){
    //ATH_CHECK( handle.record(std::unique_ptr<CscRawDataContainer>( new CscRawDataContainer(m_muonMgr->cscIdHelper()->module_hash_max())) ));
    ATH_CHECK( rdoContainerHandle.record(std::make_unique<CscRawDataContainer>( m_muonMgr->cscIdHelper()->module_hash_max() )));
    ATH_MSG_DEBUG( "Created CSCRawDataContainer" );
  }
  else{
    SG::UpdateHandle<CscRawDataCollection_Cache> update(m_rdoContainerCacheKey, ctx);
    ATH_CHECK(update.isValid());
    ATH_CHECK(rdoContainerHandle.record (std::make_unique<CscRawDataContainer>( update.ptr() )));
    ATH_MSG_DEBUG("Created container using cache for " << m_rdoContainerCacheKey.key());
  }
  
  CscRawDataContainer* container = rdoContainerHandle.ptr();

  std::set<uint32_t> robIdSet;
  SG::ReadHandle<xAOD::EventInfo> eventInfo (m_eventInfoKey, ctx);

  ATH_MSG_DEBUG ( "Before processing numColls="<<container->numberOfCollections() );

  ATH_MSG_DEBUG ( "vector of ROB ID to decode: size = " << vecRobs.size() );

  for (const ROBFragment* frag : vecRobs) {
    uint32_t robid = frag->rod_source_id();

    // check if this ROBFragment was already decoded (EF case in ROIs
    if (!robIdSet.insert(robid).second) {
      ATH_MSG_DEBUG ( " ROB Fragment with ID  " << std::hex<<robid<<std::dec<< " already decoded, skip" );
    } else {
      m_decoder->fillCollection(*eventInfo, *frag, *container);
    }
  }

  ATH_MSG_DEBUG ( "After processing numColls="<<container->numberOfCollections() );

  return StatusCode::SUCCESS;
}
