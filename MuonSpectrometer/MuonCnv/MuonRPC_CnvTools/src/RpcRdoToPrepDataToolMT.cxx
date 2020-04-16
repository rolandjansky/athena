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
  declareProperty("RpcPrdContainerCacheKey", m_prdContainerCacheKey, 
    "Optional external cache for the RPC PRD container");
  declareProperty("RpcCoinDataContainerCacheKey", m_coindataContainerCacheKey, 
    "Optional external cache for the RPC coin data container");
}

Muon::RpcRdoToPrepDataToolMT::~RpcRdoToPrepDataToolMT()
{
}

StatusCode Muon::RpcRdoToPrepDataToolMT::initialize() 
{
  ATH_MSG_VERBOSE("Starting init");
  ATH_CHECK( RpcRdoToPrepDataToolCore::initialize() );
  ATH_CHECK( m_prdContainerCacheKey.initialize( SG::AllowEmpty ) );
  ATH_CHECK( m_coindataContainerCacheKey.initialize( SG::AllowEmpty ) );
  ATH_MSG_DEBUG("initialize() successful in " << name());
  
  return StatusCode::SUCCESS;
}

StatusCode Muon::RpcRdoToPrepDataToolMT::finalize()
{   
  return RpcRdoToPrepDataToolCore::finalize();
}

StatusCode Muon::RpcRdoToPrepDataToolMT::manageOutputContainers(bool& firstTimeInTheEvent)
{
  // firstTimeInTheEvent is meaningless in MT
  // We will need to retrieve from cache even in different threads
  SG::WriteHandle< Muon::RpcPrepDataContainer >rpcPRDHandle(m_rpcPrepDataContainerKey);
  // In MT, we always want to treat this as if its the first time in the event
  firstTimeInTheEvent = true;

  // Caching of PRD container
  const bool externalCachePRD = !m_prdContainerCacheKey.key().empty();

  if (!externalCachePRD) {
    // without the cache we just record the container
    StatusCode status = rpcPRDHandle.record(std::make_unique<Muon::RpcPrepDataContainer>(m_muonIdHelperTool->rpcIdHelper().module_hash_max()));
    if (status.isFailure() || !rpcPRDHandle.isValid() )   {
      ATH_MSG_FATAL("Could not record container of RPC PrepData Container at " << m_rpcPrepDataContainerKey.key()); 
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Created container " << m_rpcPrepDataContainerKey.key());
  } 
  else {
    // use the cache to get the container
    SG::UpdateHandle<RpcPrepDataCollection_Cache> update(m_prdContainerCacheKey);
    if (!update.isValid()){
      ATH_MSG_FATAL("Invalid UpdateHandle " << m_prdContainerCacheKey.key());
      return StatusCode::FAILURE;
    }
    StatusCode status = rpcPRDHandle.record(std::make_unique<Muon::RpcPrepDataContainer>(update.ptr()));
    if (status.isFailure() || !rpcPRDHandle.isValid() )   {
      ATH_MSG_FATAL("Could not record container of RPC PrepData Container using cache " 
        << m_prdContainerCacheKey.key() << " - " <<m_rpcPrepDataContainerKey.key()); 
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Created container using cache for " << m_prdContainerCacheKey.key());
  }
  // Pass the container from the handle
  m_rpcPrepDataContainer = rpcPRDHandle.ptr();

  // Handle coin data if being used
  if (m_producePRDfromTriggerWords){
    SG::WriteHandle< Muon::RpcCoinDataContainer >rpcCoinHandle(m_rpcCoinDataContainerKey);
    const bool externalCacheCoinData = !m_coindataContainerCacheKey.key().empty();
    if(!externalCacheCoinData){
      // without the cache we just record the container
      StatusCode status = rpcCoinHandle.record(std::make_unique<Muon::RpcCoinDataContainer>(m_muonIdHelperTool->rpcIdHelper().module_hash_max()));
      if (status.isFailure() || !rpcCoinHandle.isValid() )   {
        ATH_MSG_FATAL("Could not record container of RPC Coin Data Container at " << m_rpcCoinDataContainerKey.key()); 
        return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG("Created container " << m_rpcCoinDataContainerKey.key());
    } 
    else {
      // use the cache to get the container
      SG::UpdateHandle<RpcCoinDataCollection_Cache> update(m_coindataContainerCacheKey);
      if (!update.isValid()){
        ATH_MSG_FATAL("Invalid UpdateHandle " << m_coindataContainerCacheKey.key());
        return StatusCode::FAILURE;
      }
      StatusCode status = rpcCoinHandle.record(std::make_unique<Muon::RpcCoinDataContainer>(update.ptr()));
      if (status.isFailure() || !rpcCoinHandle.isValid() )   {
        ATH_MSG_FATAL("Could not record container of RPC Coin Data Container using cache " 
          << m_coindataContainerCacheKey.key() << " - " <<m_rpcCoinDataContainerKey.key()); 
        return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG("Created container using cache for " << m_coindataContainerCacheKey.key());
    }
    // Pass the container from the handle
    m_rpcCoinDataContainer = rpcCoinHandle.ptr();
  }

  return StatusCode::SUCCESS;
}
