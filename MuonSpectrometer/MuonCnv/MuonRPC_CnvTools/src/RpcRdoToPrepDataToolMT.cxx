/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "RpcRdoToPrepDataToolMT.h"

#include "MuonRPC_CnvTools/IRPC_RDO_Decoder.h"
#include "MuonTrigCoinData/RpcCoinDataContainer.h"
#include "MuonCnvToolInterfaces/IDC_Helper.h"

//***
// Plan with this MT code is to allow the const-cast approach to occur in Core in a single-thread manner
// and once a container local only to this thread is filled, we will take its contents and cross-check 
// against the cache and insert any members not present. 
// Smarter behaviour can then be implemented to prevent too much duplicate computation but due to the 
// complexities of the Core decoding, this is the more reasonable approach to a first MT-safe implementation
//***

Muon::RpcRdoToPrepDataToolMT::MyState::MyState (const RpcIdHelper& idHelper,
                                                MsgStream& msg)
  : m_localPrepData (idHelper.module_hash_max()),
    m_localCoinData (idHelper.module_hash_max()),
    m_getPrepCollection ([&] (Identifier id) -> Muon::RpcPrepDataCollection*
      {
        Muon::RpcPrepDataCollection*& mappedCollection = m_rpcPrepDataCollections[id];
        if (!mappedCollection) {
          mappedCollection =  Muon::IDC_Helper::addCollection<RpcPrepDataContainer, RpcIdHelper>
            (id, m_rpcPrepDataContainer, idHelper, msg);
        }
        return mappedCollection;
      }),
    m_getCoinCollection ([&] (Identifier id) -> Muon::RpcCoinDataCollection*
      {
        Muon::RpcCoinDataCollection*& mappedCollection = m_rpcCoinDataCollections[id];
        if (!mappedCollection) {
          mappedCollection = Muon::IDC_Helper::addCollection<RpcCoinDataContainer, RpcIdHelper>
            (id, m_rpcCoinDataContainer, idHelper, msg);
        }
        return mappedCollection;
      })
{
  m_rpcPrepDataContainer = &m_localPrepData;
  m_rpcCoinDataContainer = &m_localCoinData;
}

Muon::RpcRdoToPrepDataToolMT::RpcRdoToPrepDataToolMT( const std::string& type, const std::string& name,
						  const IInterface* parent ) 
  : base_class( type, name, parent )
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
  ATH_CHECK( RpcRdoToPrepDataToolCore::finalize() );
  return StatusCode::SUCCESS;
}


/// This code is thread-safe as we will propagate local thread collection contents to a thread-safe one
StatusCode Muon::RpcRdoToPrepDataToolMT::decode ( std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& selectedIdVect ) const
{
  ATH_MSG_DEBUG("Calling Core decode function from MT decode function (hash vector)");
  MyState state (m_idHelperSvc->rpcIdHelper(), msg());

  ATH_CHECK( decodeImpl( state,
                         state.m_getPrepCollection,
                         state.m_getCoinCollection,
                         idVect, selectedIdVect, true ) );
  ATH_MSG_DEBUG("Core decode processed in MT decode (hash vector)");

  ATH_CHECK( transferAndRecordPrepData (state.m_localPrepData) );
  ATH_CHECK( transferAndRecordCoinData (state.m_localCoinData) );
  return StatusCode::SUCCESS;
}

/// This code is thread-safe as we will propagate local thread collection contents to a thread-safe one
StatusCode Muon::RpcRdoToPrepDataToolMT::decode ( const std::vector<uint32_t>& robIds ) const
{
  ATH_MSG_DEBUG("Calling Core decode function from MT decode function (ROB vector)");
  MyState state (m_idHelperSvc->rpcIdHelper(), msg());

  ATH_CHECK( decodeImpl( state,
                         state.m_getPrepCollection,
                         state.m_getCoinCollection,
                         robIds, true ) );
  ATH_MSG_DEBUG("Core decode processed in MT decode (ROB vector)");

  ATH_CHECK( transferAndRecordPrepData (state.m_localPrepData) );
  ATH_CHECK( transferAndRecordCoinData (state.m_localCoinData) );
  return StatusCode::SUCCESS;
}

StatusCode
Muon::RpcRdoToPrepDataToolMT::transferAndRecordPrepData (Muon::RpcPrepDataContainer& localContainer) const
{
  // We will need to retrieve from cache even in different threads
  SG::WriteHandle< Muon::RpcPrepDataContainer >rpcPRDHandle(m_rpcPrepDataContainerKey);

  // Caching of PRD container
  const bool externalCachePRD = !m_prdContainerCacheKey.key().empty();

  if (!externalCachePRD) {
    // without the cache we just record the container
    StatusCode status = rpcPRDHandle.record(std::make_unique<Muon::RpcPrepDataContainer>(m_idHelperSvc->rpcIdHelper().module_hash_max()));
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

  // Take localContainer and transfer contents to rpcPRDHandle
  auto prd_hashes = localContainer.GetAllCurrentHashes(); //deliberately GetAllCurrentHashes
  for (auto hash : prd_hashes) {
    // Remove collection from local-thread container and place into unique_ptr to move to cache
    std::unique_ptr<Muon::RpcPrepDataCollection> coll ( localContainer.removeCollection(hash) );
    // Check if it is present in the cache container
    bool isHashInCache = rpcPRDHandle->tryAddFromCache(hash);
    if (isHashInCache) {
      ATH_MSG_DEBUG("PRD hash " << hash << " exists inside cache container");
      continue;
    }
    // If not present, get a write lock for the hash and move collection
    RpcPrepDataContainer::IDC_WriteHandle lock = rpcPRDHandle->getWriteHandle( hash );
    ATH_CHECK( lock.addOrDelete(std::move( coll ) ) );
    ATH_MSG_DEBUG("PRD hash " << hash << " has been moved to cache container");
  }

  if (msgLvl(MSG::DEBUG)){
    for (const auto &[hash, ptr] : rpcPRDHandle->GetAllHashPtrPair()){
      ATH_MSG_DEBUG("Contents of CONTAINER in this view : " << hash);
    }
  }

  // For additional information on the contents of the cache-based container, this function can be used
  //printMTPrepData (*rpcPRDHandle);

  return StatusCode::SUCCESS;
}


StatusCode
Muon::RpcRdoToPrepDataToolMT::transferAndRecordCoinData (Muon::RpcCoinDataContainer& localContainer) const
{
  if (!m_producePRDfromTriggerWords) {
    return StatusCode::SUCCESS;
  }

  // Handle coin data if being used
  SG::WriteHandle< Muon::RpcCoinDataContainer >rpcCoinHandle(m_rpcCoinDataContainerKey);
  const bool externalCacheCoinData = !m_coindataContainerCacheKey.key().empty();
  if(!externalCacheCoinData){
    // without the cache we just record the container
    StatusCode status = rpcCoinHandle.record(std::make_unique<Muon::RpcCoinDataContainer>(m_idHelperSvc->rpcIdHelper().module_hash_max()));
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

  // Take localContainer and transfer contents to rpcCoinHandle
  auto coin_hashes = localContainer.GetAllCurrentHashes(); //deliberately GetAllCurrentHashes
  for (auto hash : coin_hashes) {
    // Remove collection from local-thread container and place into unique_ptr to move to cache
    std::unique_ptr<Muon::RpcCoinDataCollection> coll ( localContainer.removeCollection(hash) );
    // Check if it is present in the cache container
    bool isHashInCache = rpcCoinHandle->tryAddFromCache(hash);
    if (isHashInCache) {
      ATH_MSG_DEBUG("Coin hash " << hash << " exists inside cache container");
      continue;
    }
    // If not present, get a write lock for the hash and move collection
    RpcCoinDataContainer::IDC_WriteHandle lock = rpcCoinHandle->getWriteHandle( hash );
    ATH_CHECK( lock.addOrDelete(std::move( coll ) ) );
    ATH_MSG_DEBUG("Coin hash " << hash << " has been moved to cache container");
  }

  if (msgLvl(MSG::DEBUG)){
    for (const auto &[hash, ptr] : rpcCoinHandle->GetAllHashPtrPair()){
      ATH_MSG_DEBUG("Contents of LOCAL in this view : " << hash);
    }
  }

  // For additional information on the contents of the cache-based container, this function can be used
  //printMTCoinData (*rpcCoinHandle);
  
  return StatusCode::SUCCESS;
}


void Muon::RpcRdoToPrepDataToolMT::printMTPrepData (Muon::RpcPrepDataContainer& prepData) const
{
  msg (MSG::INFO) << "********************************************************************************************************" << endmsg;
  msg (MSG::INFO) << "***************** Listing RpcPrepData collections content **********************************************" << endmsg;
  
  if (prepData.size() <= 0)msg (MSG::INFO) << "No RpcPrepRawData collections found" << endmsg;
  
  int ncoll = 0;
  int ict = 0;
  int ictphi = 0;
  int ictamb = 0;
  int icteta = 0;
  int icttrg = 0;
  msg (MSG::INFO) <<"--------------------------------------------------------------------------------------------"<<endmsg;
  for (IdentifiableContainer<Muon::RpcPrepDataCollection>::const_iterator rpcColli = prepData.begin();
       rpcColli!=prepData.end(); ++rpcColli) {

    const Muon::RpcPrepDataCollection* rpcColl = *rpcColli;
        
    if ( rpcColl->size() > 0 ) {
      msg (MSG::INFO) <<"PrepData Collection ID "<<m_idHelperSvc->rpcIdHelper().show_to_string(rpcColl->identify())<<endmsg;
      RpcPrepDataCollection::const_iterator it_rpcPrepData;
      int icc = 0;
      int iccphi = 0;
      int icceta = 0;
      for (const RpcPrepData* rpc : *rpcColl) {
	icc++;
	ict++;
	if (m_idHelperSvc->rpcIdHelper().measuresPhi(rpc->identify())) {
	  iccphi++;
	  ictphi++;
	  if (rpc->ambiguityFlag()>1) ictamb++;
	}
	else {    
	  icceta++;
	  icteta++;
	}                    
	msg (MSG::INFO) <<ict<<" in this coll. "<<icc<<" prepData id = "
			<<m_idHelperSvc->rpcIdHelper().show_to_string(rpc->identify())
			<<" time "<<rpc->time()
			<<" ambiguityFlag "<<rpc->ambiguityFlag()<<endmsg;
      }
      ncoll++;
      msg (MSG::INFO) <<"*** Collection "<<ncoll<<" Summary: "
		      <<iccphi<<" phi hits / "
		      <<icceta<<" eta hits "<<endmsg;
      msg (MSG::INFO) <<"--------------------------------------------------------------------------------------------"<<endmsg;
    }
  }
  msg (MSG::INFO) <<"*** Event  Summary: "
		  <<ncoll <<" Collections / "
		  <<icttrg<<" trigger hits / "
		  <<ictphi<<" phi hits / "
		  <<icteta<<" eta hits "<<endmsg;
  msg (MSG::INFO) <<"--------------------------------------------------------------------------------------------"<<endmsg;
}

void Muon::RpcRdoToPrepDataToolMT::printMTCoinData (Muon::RpcCoinDataContainer& coinData) const
{
  msg (MSG::INFO) << "********************************************************************************************************" << endmsg;
  msg (MSG::INFO) << "***************** Listing RpcCoinData collections content **********************************************" << endmsg;
  
  if (coinData.size() <= 0)msg (MSG::INFO) << "No RpcCoinData collections found" << endmsg;
  
  int ncoll = 0;
  int ict = 0;
  int ictphi = 0;
  int icteta = 0;
  int ictphilc = 0;
  int ictphihc = 0;
  int ictetalc = 0;
  int ictetahc = 0;
  msg (MSG::INFO) <<"--------------------------------------------------------------------------------------------"<<endmsg;
  for (IdentifiableContainer<Muon::RpcCoinDataCollection>::const_iterator rpcColli = coinData.begin();
       rpcColli!=coinData.end(); ++rpcColli) {

    const Muon::RpcCoinDataCollection* rpcColl = *rpcColli;
        
    if ( rpcColl->size() > 0 ) {  
      msg (MSG::INFO) <<"CoinData Collection ID "<<m_idHelperSvc->rpcIdHelper().show_to_string(rpcColl->identify())<<endmsg;
      RpcCoinDataCollection::const_iterator it_rpcCoinData;
      int icc = 0;
      int iccphi = 0;
      int icceta = 0;
      int iccphilc = 0;
      int iccetahc = 0;
      int iccphihc = 0;
      int iccetalc = 0;

      for (const RpcCoinData* rpc : *rpcColl) {
	icc++;
	ict++;

	if (m_idHelperSvc->rpcIdHelper().measuresPhi(rpc->identify())) {
	        
	  iccphi++;
	  ictphi++;
	  if ( rpc->isLowPtCoin() ) {
	    iccphilc++;
	    ictphilc++;
	  }
	  else if (rpc->isHighPtCoin()) {
	    iccphihc++;
	    ictphihc++;
	  }                    
	}
	else {
	  icceta++;
	  icteta++;
	  if ( rpc->isLowPtCoin() ) {
	    iccetalc++;
	    ictetalc++;
	  }
	  else if (rpc->isHighPtCoin()) {
	    iccetahc++;
	    ictetahc++;
	  }
	}                    
	msg (MSG::INFO) <<ict<<" in this coll. "<<icc<<" coinData id = "
			<<m_idHelperSvc->rpcIdHelper().show_to_string(rpc->identify())
			<<" time "<<rpc->time()<<" ijk = "<<rpc->ijk()
			<<" cm/pad/sl ids = "
			<<rpc->parentCmId()<<"/"<<rpc->parentPadId()<<"/"<<rpc->parentSectorId()<<"/"
			<<" isLowPtCoin/HighPtCoin/LowPtInputToHighPt "
			<<rpc->isLowPtCoin() <<"/"<<rpc->isHighPtCoin() <<"/"<<rpc->isLowPtInputToHighPtCm() 
			<<endmsg;
      }
      ncoll++;
      msg (MSG::INFO) <<"*** Collection "<<ncoll<<" Summary: "
		      <<iccphi<<" phi coin. hits / "
		      <<icceta<<" eta coin. hits \n"
		      <<iccphilc<<" phi lowPt / "
		      <<iccphihc<<" phi highPt / "
		      <<iccetalc<<" eta lowPt / "
		      <<iccetahc<<" eta highPt coincidences  "
		      <<endmsg;
      msg (MSG::INFO) <<"--------------------------------------------------------------------------------------------"<<endmsg;
    }
  }
  msg (MSG::INFO) <<"*** Event  Summary: "
		  <<ncoll <<" Collections / "
		  <<ictphi<<" phi coin. hits / "
		  <<icteta<<" eta coin. hits \n"
		  <<ictphilc<<" phi lowPt / "
		  <<ictphihc<<" phi highPt / "
		  <<ictetalc<<" eta lowPt / "
		  <<ictetahc<<" eta highPt coincidences  "
		  <<endmsg;
  msg (MSG::INFO) <<"--------------------------------------------------------------------------------------------"<<endmsg;
}


void Muon::RpcRdoToPrepDataToolMT::printPrepData() const
{
  const EventContext& ctx = Gaudi::Hive::currentContext();

  SG::ReadHandleKey<Muon::RpcPrepDataContainer> prepDataKey (m_rpcPrepDataContainerKey.key());
  prepDataKey.initialize().ignore();
  
  SG::ReadHandleKey<Muon::RpcCoinDataContainer> prepCoinKey (m_rpcCoinDataContainerKey.key());
  prepCoinKey.initialize().ignore();
  
  printPrepDataImpl(*SG::makeHandle(prepDataKey, ctx).get(),
                    *SG::makeHandle(prepCoinKey, ctx).get());
}
