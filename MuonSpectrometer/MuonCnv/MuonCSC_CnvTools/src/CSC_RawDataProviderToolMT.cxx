/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CSC_RawDataProviderToolMT.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "CSC_RawDataProviderToolMT.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "ByteStreamData/ROBData.h"

#include "MuonIdHelpers/CscIdHelper.h"

#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamData/RawEvent.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;


//================ Constructor =================================================

Muon::CSC_RawDataProviderToolMT::CSC_RawDataProviderToolMT(const std::string& t,
                                                       const std::string& n,
                                                       const IInterface*  p) :
  CSC_RawDataProviderToolCore(t, n, p)
{
  declareInterface<IMuonRawDataProviderTool>(this);

  declareProperty ("CscContainerCacheKey", m_rdoContainerCacheKey, "Optional external cache for the CSC container");

}

//================ Destructor =================================================

Muon::CSC_RawDataProviderToolMT::~CSC_RawDataProviderToolMT()
{}

//================ Initialisation =================================================

StatusCode Muon::CSC_RawDataProviderToolMT::initialize()
{
  // call initialize from base class
  ATH_CHECK( CSC_RawDataProviderToolCore::initialize() );

  // Initialise the container cache if available  
  ATH_CHECK( m_rdoContainerCacheKey.initialize( !m_rdoContainerCacheKey.key().empty() ) );

  ATH_CHECK(m_idHelperSvc.retrieve());

  return StatusCode::SUCCESS;
}

//============================================================================================

// new one
StatusCode Muon::CSC_RawDataProviderToolMT::convert(const std::vector<IdentifierHash>& rdoIdhVect){

  IdContext cscContext = m_idHelperSvc->cscIdHelper().module_context();

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

StatusCode Muon::CSC_RawDataProviderToolMT::convert(const EventContext& ctx) const {
  
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
  const std::vector< uint32_t >& robIds = m_hid2re.allRobIds();
  ATH_MSG_VERBOSE ( "Number of ROB ids " << robIds.size() );
  // ask ROBDataProviderSvc for the vector of ROBFragment for all MDT ROBIDs
  m_robDataProvider->getROBData( robIds, vecOfRobf);
  ATH_MSG_VERBOSE ( "Number of ROB fragments " << vecOfRobf.size() );

  return convert(vecOfRobf, ctx);
}


StatusCode Muon::CSC_RawDataProviderToolMT::convert(const ROBFragmentList& vecRobs,
                                                  const std::vector<IdentifierHash>& /* collections */){
  const CSC_RawDataProviderToolMT* cthis = this;
  return cthis->convert (vecRobs, Gaudi::Hive::currentContext());
}

StatusCode
Muon::CSC_RawDataProviderToolMT::convert(const ROBFragmentList& vecRobs,
                                       const EventContext& ctx) const
{

  SG::WriteHandle<CscRawDataContainer> rdoContainerHandle(m_containerKey, ctx);

  // Split the methods to have one where we use the cache and one where we just setup the container
  const bool externalCacheRDO = !m_rdoContainerCacheKey.key().empty();
  if(!externalCacheRDO){
    ATH_CHECK( rdoContainerHandle.record(std::make_unique<CscRawDataContainer>( m_idHelperSvc->cscIdHelper().module_hash_max() )));
    ATH_MSG_DEBUG( "Created CSCRawDataContainer" );
  }
  else{
    SG::UpdateHandle<CscRawDataCollection_Cache> update(m_rdoContainerCacheKey, ctx);
    ATH_CHECK(update.isValid());
    ATH_CHECK(rdoContainerHandle.record (std::make_unique<CscRawDataContainer>( update.ptr() )));
    ATH_MSG_DEBUG("Created container using cache for " << m_rdoContainerCacheKey.key());
  }
  
  CscRawDataContainer* container = rdoContainerHandle.ptr();

  if(!container) {
    ATH_MSG_ERROR("CSC RDO container pointer is null, cannot decode data");
    return StatusCode::FAILURE;
  }

  // call conversion function from the base class
  ATH_CHECK( convertIntoContainer(vecRobs, ctx, *container) );

  return StatusCode::SUCCESS;
}
