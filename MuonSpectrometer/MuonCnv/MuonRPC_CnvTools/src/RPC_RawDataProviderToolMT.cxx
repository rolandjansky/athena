/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_RawDataProviderToolMT.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"

// #include "valgrind/callgrind.h"

#define padMaxIndex 600
#define slMaxIndex  200


using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

Muon::RPC_RawDataProviderToolMT::RPC_RawDataProviderToolMT(
    const std::string& t,
    const std::string& n,
    const IInterface*  p) :
    RPC_RawDataProviderToolCore(t, n, p)
{
  declareInterface<IMuonRawDataProviderTool>(this);
  declareProperty("RpcContainerCacheKey", m_rdoContainerCacheKey, "Optional external cache for the RPC container");
}


Muon::RPC_RawDataProviderToolMT::~RPC_RawDataProviderToolMT()
{}


StatusCode Muon::RPC_RawDataProviderToolMT::initialize()
{
    ATH_CHECK(RPC_RawDataProviderToolCore::initialize());

    ATH_CHECK( m_rdoContainerCacheKey.initialize( !m_rdoContainerCacheKey.key().empty() ) );

    // We should only turn off the sector logic when running with cached data a la trigger mode
    if(!m_rdoContainerCacheKey.key().empty() && m_WriteOutRpcSectorLogic){
      ATH_MSG_FATAL("Cannot write out RpcSectorLogic while running with cached RpcPad containers" 
        " as the RpcSectorLogic is not cached at the same time and the written containers will desync." 
        " Please turn off RpcSectorLogic writing when running with cached bytestream container");
      return StatusCode::FAILURE;
    }
        
    ATH_MSG_INFO( "initialize() successful in " << name());
    
    return StatusCode::SUCCESS;
}


// the new one 
StatusCode Muon::RPC_RawDataProviderToolMT::convert()
{
  SG::ReadCondHandle<RpcCablingCondData> readHandle{m_readKey};
  const RpcCablingCondData* readCdo{*readHandle};
//CALLGRIND_START_INSTRUMENTATION
  /// 
  m_decoder->setSLdecodingRequest();
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
  std::vector<uint32_t> robIds = readCdo->giveFullListOfRobIds();
  m_robDataProvider->getROBData( robIds, vecOfRobf);
//CALLGRIND_STOP_INSTRUMENTATION
  return convert(vecOfRobf); // using the old one
}
// the old one 
StatusCode Muon::RPC_RawDataProviderToolMT::convert(const ROBFragmentList& vecRobs)
{
 //CALLGRIND_START_INSTRUMENTATION
    std::vector<IdentifierHash> collections;
 //CALLGRIND_STOP_INSTRUMENTATION
    return convert(vecRobs,collections); 
}

// the new one 
StatusCode Muon::RPC_RawDataProviderToolMT::convert(const std::vector<uint32_t>& robIds)
{
 //CALLGRIND_START_INSTRUMENTATION
    std::vector<IdentifierHash> collections;
    std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
    m_robDataProvider->getROBData(robIds, vecOfRobf);
 //CALLGRIND_STOP_INSTRUMENTATION
    return convert(vecOfRobf,collections); 
}

// the new one
StatusCode Muon::RPC_RawDataProviderToolMT::convert(const std::vector<IdentifierHash>& rdoIdhVect)
{
  SG::ReadCondHandle<RpcCablingCondData> readHandle{m_readKey};
  const RpcCablingCondData* readCdo{*readHandle};
 //CALLGRIND_START_INSTRUMENTATION
    std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
    std::vector<uint32_t> robIds;
    CHECK( readCdo->giveROB_fromRDO(rdoIdhVect, robIds) );
    m_robDataProvider->getROBData(robIds, vecOfRobf);
//CALLGRIND_STOP_INSTRUMENTATION
    return convert(vecOfRobf, rdoIdhVect); // using the old one 
}
// the old one 
StatusCode Muon::RPC_RawDataProviderToolMT::convert(const ROBFragmentList& vecRobs,
  const std::vector<IdentifierHash>& collections)
{
 //CALLGRIND_START_INSTRUMENTATION

  SG::WriteHandle<RpcPadContainer> rdoContainerHandle(m_containerKey);
  SG::WriteHandle<RpcSectorLogicContainer> logicHandle(m_sec);

  // run 3 mode, here we always try to write the containers

  // Split the methods to have one where we use the cache and one where we just setup the container
  const bool externalCacheRDO = !m_rdoContainerCacheKey.key().empty();
  if(!externalCacheRDO){
    ATH_CHECK( rdoContainerHandle.record(std::make_unique<RpcPadContainer> (padMaxIndex) ) );
    ATH_MSG_DEBUG( "Created RpcPadContainer" );
  }
  else{
    SG::UpdateHandle<RpcPad_Cache> update(m_rdoContainerCacheKey);
    ATH_CHECK(update.isValid());
    ATH_CHECK(rdoContainerHandle.record (std::make_unique<RpcPadContainer>( update.ptr() )));
    ATH_MSG_DEBUG("Created container using cache for " << m_rdoContainerCacheKey.key());
  }
  RpcPadContainer* pad = rdoContainerHandle.ptr();
  
  RpcSectorLogicContainer* logic = 0;
  if(m_WriteOutRpcSectorLogic) {
    ATH_MSG_DEBUG("Writing out RpcSectorLogicContainer");
    ATH_CHECK( logicHandle.record (std::make_unique<RpcSectorLogicContainer>()) );
    logic = logicHandle.ptr();
  }

  // pass the containers to the convertIntoContainers function in the RPC_RawDataProviderToolCore base class
  ATH_CHECK( convertIntoContainers(vecRobs, collections, pad, logic) );

  return StatusCode::SUCCESS;
}







