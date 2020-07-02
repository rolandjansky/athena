/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RpcRdoToRpcDigit.h"

RpcRdoToRpcDigit::RpcRdoToRpcDigit(const std::string& name,
                                   ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode RpcRdoToRpcDigit::initialize()
{
  ATH_CHECK(m_idHelperSvc.retrieve());

  // get RPC cabling
  ATH_CHECK(m_rpcCablingServerSvc.retrieve());
  ATH_CHECK(m_rpcCablingServerSvc->giveCabling(m_rpcCabling) );

  ATH_CHECK(m_rpcRdoKey.initialize());
  ATH_CHECK(m_rpcDigitKey.initialize());

  ATH_CHECK( m_rpcRdoDecoderTool.retrieve() );

  ATH_CHECK(m_rpcReadKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode RpcRdoToRpcDigit::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG( "in execute()"  );
  // retrieve the collection of RDO
  SG::ReadHandle<RpcPadContainer> rdoRH(m_rpcRdoKey, ctx);
  if (!rdoRH.isValid()) {
    ATH_MSG_WARNING( "No RPC RDO container found!"  );
    return StatusCode::SUCCESS;
  }
  const RpcPadContainer* rdoContainer = rdoRH.cptr();
  ATH_MSG_DEBUG( "Retrieved " << rdoContainer->size() << " RPC RDOs." );

  SG::WriteHandle<RpcDigitContainer> wh_rpcDigit(m_rpcDigitKey, ctx);
  ATH_CHECK(wh_rpcDigit.record(std::make_unique<RpcDigitContainer> (m_idHelperSvc->rpcIdHelper().module_hash_max())));
  ATH_MSG_DEBUG( "Decoding RPC RDO into RPC Digit"  );

  RpcDigitCollection * collection = nullptr;

  // now decode RDO into digits
  RpcPadContainer::const_iterator rpcPAD = rdoContainer->begin();

  SG::ReadCondHandle<RpcCablingCondData> cablingCondData{m_rpcReadKey, ctx};
  const RpcCablingCondData* rpcCabling{*cablingCondData};

  for (; rpcPAD!=rdoContainer->end();++rpcPAD) {
    if ( (*rpcPAD)->size() > 0 ) {
      ATH_CHECK(this->decodeRpc(*rpcPAD, wh_rpcDigit.ptr(), collection, rpcCabling));
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode RpcRdoToRpcDigit::decodeRpc(const RpcPad * rdoColl, RpcDigitContainer *rpcContainer, RpcDigitCollection*& collection, const RpcCablingCondData* rpcCab) const
{

  const IdContext rpcContext = m_idHelperSvc->rpcIdHelper().module_context();

  ATH_MSG_DEBUG( " Number of CMs in this Pad "
                 << rdoColl->size()  );

  const Identifier padOfflineId = rdoColl->identify();
  //Get pad online id and sector id
  uint16_t padId     = rdoColl->onlineId();
  uint16_t sectorId  = rdoColl->sector();

  const int stationName = m_idHelperSvc->rpcIdHelper().stationName(padOfflineId);
  const int stationEta  = m_idHelperSvc->rpcIdHelper().stationEta(padOfflineId);
  const int stationPhi  = m_idHelperSvc->rpcIdHelper().stationPhi(padOfflineId);
  const int doubletR    = m_idHelperSvc->rpcIdHelper().doubletR(padOfflineId);

  Identifier elementId = m_idHelperSvc->rpcIdHelper().elementID(stationName, stationEta,
                                                stationPhi, doubletR);

  // For each pad, loop on the coincidence matrices
  for (const RpcCoinMatrix * coinMatrix : *rdoColl) {

    // Get CM online Id
    uint16_t cmaId = coinMatrix->onlineId();

    // For each CM, loop on the fired channels
    for (const RpcFiredChannel * rpcChan : *coinMatrix) {
      const std::vector<RpcDigit*>* digitVec =
        m_rpcRdoDecoderTool->getDigit(rpcChan, sectorId, padId, cmaId, rpcCab);

      if (!digitVec) {
        ATH_MSG_FATAL( "Error in the RPC RDO decoder "  );
        return StatusCode::FAILURE;
      }

      // Loop on the digits corresponding to the fired channel
      for (RpcDigit *newDigit : *digitVec) {
        collection = nullptr;
        elementId = m_idHelperSvc->rpcIdHelper().elementID(newDigit->identify());

        IdentifierHash coll_hash;
        if (m_idHelperSvc->rpcIdHelper().get_hash(elementId, coll_hash, &rpcContext)) {
          ATH_MSG_WARNING( "Unable to get RPC digit collection hash id "
                           << "context begin_index = " << rpcContext.begin_index()
                           << " context end_index  = " << rpcContext.end_index()
                           << " the identifier is " );
          elementId.show();
        }

        const RpcDigitCollection * coll = rpcContainer->indexFindPtr(coll_hash);
        if (nullptr ==  coll) {
          RpcDigitCollection * newCollection = new RpcDigitCollection(elementId, coll_hash);
          newCollection->push_back(newDigit);
          collection = newCollection;
          if (rpcContainer->addCollection(newCollection, coll_hash).isFailure())
            ATH_MSG_WARNING( "Couldn't record RpcDigitCollection with key="
                             << coll_hash << " in StoreGate!"  );
        }
        else
          {
            RpcDigitCollection * oldCollection ATLAS_THREAD_SAFE = const_cast<RpcDigitCollection*>( coll ); // FIXME
            oldCollection->push_back(newDigit);
            collection = oldCollection;
          }
      }
      delete digitVec;
    }
  }

  return StatusCode::SUCCESS;
}
