/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// This code is used in both MT and single-thread
/// However the MT code uses the const cast in a single-thread
/// mode by transfering the contents to a thread-safe container
/// in order to prevent complex adjustments in the core decode functions

#include "RpcRdoToPrepDataTool.h"

#include "MuonRPC_CnvTools/IRPC_RDO_Decoder.h"
#include "MuonTrigCoinData/RpcCoinDataContainer.h"
#include "MuonCnvToolInterfaces/IDC_Helper.h"


namespace {


using namespace Muon;


template <class CONT, class COLL>
struct ATLAS_NOT_THREAD_SAFE GetCollection
{
  GetCollection (const RpcIdHelper& idHelper,
                 CONT* container,
                 MsgStream& msg)
  : m_idHelper (idHelper),
    m_container (container),
    m_msg (msg)
    {}

  COLL* operator() (Identifier id)
  {
    return Muon::IDC_Helper::getCollection<CONT, RpcIdHelper>
      (id, m_container, m_idHelper, m_msg);
  }

  const RpcIdHelper& m_idHelper;
  CONT* m_container;
  MsgStream& m_msg;
};


} // anonymous namespace


Muon::RpcRdoToPrepDataTool::RpcRdoToPrepDataTool( const std::string& type, const std::string& name,
						  const IInterface* parent ) 
  : base_class( type, name, parent )
{
}

StatusCode Muon::RpcRdoToPrepDataTool::initialize() 
{
  ATH_MSG_VERBOSE("Starting init");
  ATH_CHECK( RpcRdoToPrepDataToolCore::initialize() );
  ATH_MSG_DEBUG("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Muon::RpcRdoToPrepDataTool::manageOutputContainers(bool& firstTimeInTheEvent) const
{
  SG::WriteHandle< Muon::RpcPrepDataContainer > rpcPrepDataHandle(m_rpcPrepDataContainerKey);
  if(!rpcPrepDataHandle.isPresent()) {
    firstTimeInTheEvent = true;

    StatusCode status = rpcPrepDataHandle.record(std::make_unique<Muon::RpcPrepDataContainer>(m_idHelperSvc->rpcIdHelper().module_hash_max()));

    if (status.isFailure() || !rpcPrepDataHandle.isValid() ) 	{
      ATH_MSG_FATAL("Could not record container of RPC PrepData Container at " << m_rpcPrepDataContainerKey.key());
      return status;
    } else {
      m_state.m_rpcPrepDataContainer = rpcPrepDataHandle.ptr();
      ATH_MSG_DEBUG("RPC PrepData Container recorded in StoreGate with key " << m_rpcPrepDataContainerKey.key() << ", " << rpcPrepDataHandle.key());
    }

    if (m_producePRDfromTriggerWords){
      /// create an empty RPC trigger hit container for filling
      SG::WriteHandle< Muon::RpcCoinDataContainer > rpcCoinDataHandle(m_rpcCoinDataContainerKey);
      status = rpcCoinDataHandle.record(std::make_unique<Muon::RpcCoinDataContainer>(m_idHelperSvc->rpcIdHelper().module_hash_max()));

      if (status.isFailure() || !rpcCoinDataHandle.isValid() ) 	{
        ATH_MSG_FATAL("Could not record container of RPC TrigCoinData Container at " << m_rpcCoinDataContainerKey.key());
        return status;
      } else {
        m_state.m_rpcCoinDataContainer = rpcCoinDataHandle.ptr();
	ATH_MSG_DEBUG("RPC TrigCoinData Container recorded in StoreGate with key " << m_rpcCoinDataContainerKey.key());
      }      
      ATH_MSG_VERBOSE(" RpcCoinDataContainer created");
    } 
    m_state.m_decodedOfflineHashIds.clear();
    m_state.m_decodedRobIds.clear();

  }
  else{
    const Muon::RpcPrepDataContainer* rpcPrepDataContainer_c = nullptr;
    ATH_CHECK( evtStore()->retrieve (rpcPrepDataContainer_c, m_rpcPrepDataContainerKey.key()) );
    m_state.m_rpcPrepDataContainer = const_cast<Muon::RpcPrepDataContainer*> (rpcPrepDataContainer_c);
    ATH_MSG_DEBUG("RPC PrepData Container is already in StoreGate ");
    if (m_producePRDfromTriggerWords){
      SG::WriteHandle< Muon::RpcCoinDataContainer > rpcCoinDataHandle(m_rpcCoinDataContainerKey);
      if (!rpcCoinDataHandle.isPresent()) {
        ATH_MSG_FATAL("Muon::RpcPrepDataContainer found while Muon::RpcCoinDataContainer not found in Event Store");
        return StatusCode::FAILURE;
      }
      const Muon::RpcCoinDataContainer* rpcCoinDataContainer_c = nullptr;
      ATH_CHECK( evtStore()->retrieve (rpcCoinDataContainer_c, m_rpcCoinDataContainerKey.key()) );
      m_state.m_rpcCoinDataContainer = const_cast<Muon::RpcCoinDataContainer*> (rpcCoinDataContainer_c);
      ATH_MSG_DEBUG("RPC CoinData Container is already in StoreGate ");
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode Muon::RpcRdoToPrepDataTool::decode( std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& selectedIdVect ) const
{
  bool firstTimeInTheEvent=false;
  ATH_CHECK( manageOutputContainers (firstTimeInTheEvent) );

  // In principle, these could be written as lambdas.
  // But we need gcc10 to be able to declare lambdas as not thread-safe.
  getPrepCollection_func getPrepCollection = GetCollection<RpcPrepDataContainer, RpcPrepDataCollection>
    (m_idHelperSvc->rpcIdHelper(), m_state.m_rpcPrepDataContainer, msg());
  getCoinCollection_func getCoinCollection = GetCollection<RpcCoinDataContainer, RpcCoinDataCollection>
    (m_idHelperSvc->rpcIdHelper(), m_state.m_rpcCoinDataContainer, msg());

  ATH_MSG_DEBUG("Calling Core decode function from Legacy decode function (hash vector)");
  StatusCode status = decodeImpl( m_state,
                                  getPrepCollection,
                                  getCoinCollection,
                                  idVect, selectedIdVect, firstTimeInTheEvent );
  if (status.isFailure()){
    ATH_MSG_FATAL("Error processing Core decode from Legacy (hash vector)");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Core decode processed in Legacy decode (hash vector)");
  if (msgLvl(MSG::DEBUG)){
     for (const auto &[hash, ptr] : m_state.m_rpcPrepDataContainer->GetAllHashPtrPair()){
       ATH_MSG_DEBUG("Contents of CONTAINER in this view : " << hash);
     }
  }
  // For additional information on container contents, this function can be used 
  //  Muon::RpcRdoToPrepDataToolCore::printPrepData();

  return StatusCode::SUCCESS;
}

StatusCode Muon::RpcRdoToPrepDataTool::decode( const std::vector<uint32_t>& robIds ) const
{
  bool firstTimeInTheEvent=false;
  ATH_CHECK( manageOutputContainers (firstTimeInTheEvent) );

  // In principle, these could be written as lambdas.
  // But we need gcc10 to be able to declare lambdas as not thread-safe.
  getPrepCollection_func getPrepCollection = GetCollection<RpcPrepDataContainer, RpcPrepDataCollection>
    (m_idHelperSvc->rpcIdHelper(), m_state.m_rpcPrepDataContainer, msg());
  getCoinCollection_func getCoinCollection = GetCollection<RpcCoinDataContainer, RpcCoinDataCollection>
    (m_idHelperSvc->rpcIdHelper(), m_state.m_rpcCoinDataContainer, msg());

  ATH_MSG_DEBUG("Calling Core decode function from Legacy decode function (ROB vector)");
  StatusCode status = decodeImpl( m_state,
                                  getPrepCollection,
                                  getCoinCollection,
                                  robIds, firstTimeInTheEvent );
  if (status.isFailure()){
    ATH_MSG_FATAL("Error processing Core decode from Legacy (ROB vector)");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Core decode processed in Legacy decode (ROB vector)");
  if (msgLvl(MSG::DEBUG)){
     for (const auto &[hash, ptr] : m_state.m_rpcPrepDataContainer->GetAllHashPtrPair()){
       ATH_MSG_DEBUG("Contents of CONTAINER in this view : " << hash);
     }
  }
  // For additional information on container contents, this function can be used
  //  Muon::RpcRdoToPrepDataToolCore::printPrepData();

  return StatusCode::SUCCESS;
}


void Muon::RpcRdoToPrepDataTool::printPrepData() const
{
  printPrepDataImpl(*m_state.m_rpcPrepDataContainer,
                    *m_state.m_rpcCoinDataContainer);
}
