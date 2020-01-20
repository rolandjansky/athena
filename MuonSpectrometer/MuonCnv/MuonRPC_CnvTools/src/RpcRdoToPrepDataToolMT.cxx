/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "RpcRdoToPrepDataToolMT.h"

#include "MuonRPC_CnvTools/IRPC_RDO_Decoder.h"
#include "MuonTrigCoinData/RpcCoinDataContainer.h"


Muon::RpcRdoToPrepDataToolMT::RpcRdoToPrepDataToolMT( const std::string& type, const std::string& name,
						  const IInterface* parent ) 
  : AthAlgTool( type, name, parent ),
    RpcRdoToPrepDataToolCore( type, name, parent )
{
  declareProperty("RpcRdoContainerCacheKey", m_prdContainerCacheKey, "Optional external cache for the RPC RDO container");
  declareProperty("RpcCoinContainterCacheKey", m_coinContainerCacheKey, "Optional external cache for the RPC Trigger Coin container");
}

Muon::RpcRdoToPrepDataToolMT::~RpcRdoToPrepDataToolMT()
{
}

StatusCode Muon::RpcRdoToPrepDataToolMT::initialize() 
{
  ATH_MSG_VERBOSE("Starting init");
  ATH_CHECK( RpcRdoToPrepDataToolCore::initialize() );
  ATH_CHECK( m_prdContainerCacheKey.initialize( !m_prdContainerCacheKey.key().empty() ) );
  ATH_CHECK( m_coinContainerCacheKey.initialize( !m_coinContainerCacheKey.key().empty() ) );
  ATH_MSG_DEBUG("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Muon::RpcRdoToPrepDataToolMT::finalize()
{   
  return RpcRdoToPrepDataToolCore::finalize();
}

StatusCode Muon::RpcRdoToPrepDataToolMT::manageOutputContainers(bool& firstTimeInTheEvent)
{
  firstTimeInTheEvent = false;

  // RPC PRD
  SG::WriteHandle< Muon::RpcPrepDataContainer > rpcPrepDataHandle(m_rpcPrepDataContainerKey);

  // Caching of PRD container
  const bool externalCachePRD = !m_prdContainerCacheKey.key().empty();
  if (!externalCachePRD) {
    // without the cache we just record the container
    StatusCode status = rpcPrepDataHandle.record(std::make_unique<Muon::RpcPrepDataContainer>(m_muonIdHelperTool->rpcIdHelper().module_hash_max()));
    if (status.isFailure() || !rpcPrepDataHandle.isValid() )  {
      ATH_MSG_FATAL("Could not record container of RPC PrepData Container at " << m_rpcPrepDataContainerKey.key());
      return status;
    }
    ATH_MSG_DEBUG("Created container " << m_prdContainerCacheKey.key());
  } 
  else {
    // use the cache to get the container
    SG::UpdateHandle<RpcPrepDataCollection_Cache> update(m_prdContainerCacheKey);
    if (!update.isValid()){
      ATH_MSG_FATAL("Invalid UpdateHandle " << m_prdContainerCacheKey.key());
      return StatusCode::FAILURE;
    }
    StatusCode status = rpcPrepDataHandle.record(std::make_unique<Muon::RpcPrepDataContainer>(update.ptr()));
    if (status.isFailure() || !rpcPrepDataHandle.isValid() )   {
      ATH_MSG_FATAL("Could not record container of RPC PrepData Container using cache " 
        << m_prdContainerCacheKey.key() << " - " << m_rpcPrepDataContainerKey.key()); 
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Created container using cache for " << m_prdContainerCacheKey.key());
  }
  // Pass the container from the handle
  m_rpcPrepDataContainer = rpcPrepDataHandle.ptr();

  // Cache for the RPC trigger coin
  if(m_producePRDfromTriggerWords){
    // RPC Trigger Coin
    SG::WriteHandle< Muon::RpcCoinDataContainer > rpcCoinDataHandle(m_rpcCoinDataContainerKey);
    // Caching of trigger coin container
    const bool externalCacheTriggerCoin = !m_coinContainerCacheKey.key().empty();
    if(!externalCacheTriggerCoin){
      StatusCode status = rpcCoinDataHandle.record(std::make_unique<Muon::RpcCoinDataContainer>(m_muonIdHelperTool->rpcIdHelper().module_hash_max()));
      if (status.isFailure() || !rpcCoinDataHandle.isValid() )  {
        ATH_MSG_FATAL("Could not record container of RPC TrigCoinData Container at " << m_rpcCoinDataContainerKey.key());
        return status;
      }
      // Clean if it was created
      m_decodedOfflineHashIds.clear();
      m_decodedRobIds.clear();
    }
    else{
      // use the cache to get the container
      SG::UpdateHandle<RpcCoinDataCollection_Cache> update(m_coinContainerCacheKey);
      if (!update.isValid()){
        ATH_MSG_FATAL("Invalid UpdateHandle " << m_coinContainerCacheKey.key());
        return StatusCode::FAILURE;
      }
      StatusCode status = rpcCoinDataHandle.record(std::make_unique<Muon::RpcCoinDataContainer>(update.ptr()));
      if (status.isFailure() || !rpcCoinDataHandle.isValid() )   {
        ATH_MSG_FATAL("Could not record container of RPC TrigCoinData Container using cache " 
          << m_coinContainerCacheKey.key() << " - " << m_rpcCoinDataContainerKey.key()); 
        return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG("Created container using cache for " << m_rpcCoinDataContainerKey.key());
    }
    // Pass the container from the handle
    m_rpcCoinDataContainer = rpcCoinDataHandle.ptr();
  }

  return StatusCode::SUCCESS;
}
