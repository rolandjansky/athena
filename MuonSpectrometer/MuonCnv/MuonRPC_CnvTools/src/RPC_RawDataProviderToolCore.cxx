/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "RPC_RawDataProviderToolCore.h"

#include "MuonRPC_CnvTools/IRpcROD_Decoder.h"
#include "RPCcablingInterface/RpcPadIdHash.h"
#include "eformat/SourceIdentifier.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;
using eformat::helper::SourceIdentifier;

Muon::RPC_RawDataProviderToolCore::RPC_RawDataProviderToolCore(
    const std::string& t,
    const std::string& n,
    const IInterface*  p) :
    AthAlgTool(t, n, p),
    m_robDataProvider ("ROBDataProviderSvc",n),
    m_decoder("Muon::RpcROD_Decoder/RpcROD_Decoder", this)
{
    declareProperty("Decoder",     m_decoder);

}

Muon::RPC_RawDataProviderToolCore::~RPC_RawDataProviderToolCore() 
{

}


StatusCode Muon::RPC_RawDataProviderToolCore::initialize()
{
  // retrieve ROD decoder
  ATH_CHECK(m_decoder.retrieve());

  ATH_CHECK( m_containerKey.initialize() );
  ATH_CHECK( m_sec.initialize() );

  // get cabling svc
  const IRPCcablingServerSvc* RpcCabGet = 0;
  StatusCode sc = service("RPCcablingServerSvc", RpcCabGet);
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Could not get RPCcablingServerSvc !" );
    return StatusCode::FAILURE;
  }
  else ATH_MSG_VERBOSE( " RPCcablingServerSvc retrieved" );
  
  sc = RpcCabGet->giveCabling(m_rpcCabling);
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Could not get RPCcablingSvc from the Server !" );
    m_rpcCabling = 0;
    return StatusCode::FAILURE;
  } 
  else {
    ATH_MSG_VERBOSE( " RPCcablingSvc obtained " );
  }

  // get ROB data provider service
  ATH_CHECK(m_robDataProvider.retrieve());

  return sc;
}

StatusCode Muon::RPC_RawDataProviderToolCore::convertIntoContainers(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs,  const std::vector<IdentifierHash>& collections,
                                                    RpcPadContainer* pad, RpcSectorLogicContainer* logic) const {

  for (auto itFrag = vecRobs.begin(); itFrag != vecRobs.end(); itFrag++)
  {
    //convert only if data payload is delivered
    if ( (**itFrag).rod_ndata()!=0 )
    {
      std::vector<IdentifierHash> coll = to_be_converted(**itFrag,collections);

      if (m_decoder->fillCollections(**itFrag, *pad, coll, logic).isFailure())
      {
        // store the error conditions into the StatusCode and continue
      }
    }
    else
    {
     if(msgLvl(MSG::DEBUG))
     {
        uint32_t sourceId= (**itFrag).source_id();
        msg(MSG::DEBUG) << " ROB " << MSG::hex << sourceId
        << " is delivered with an empty payload" << MSG::dec 
        << endmsg;
      }
      // store the error condition into the StatusCode and continue
    }
  }

  ATH_MSG_DEBUG("After processing, number of collections in container : " << pad-> numberOfCollections() );

  //in presence of errors return FAILURE
  //CALLGRIND_STOP_INSTRUMENTATION
  return StatusCode::SUCCESS;
}

std::vector<IdentifierHash> Muon::RPC_RawDataProviderToolCore::to_be_converted(const ROBFragment& robFrag,
                                                                               const std::vector<IdentifierHash>& coll) const
{
    std::vector<IdentifierHash> to_return;
    if(coll.empty())
    {
    // get SubdetectorId and ModuleId
        uint32_t source_id = robFrag.rod_source_id();
        SourceIdentifier sid(source_id);
        to_return =
            (m_rpcCabling->padHashFunction())->rod2hash(sid.subdetector_id(), sid.module_id() ); 
    }
    else
    {
        uint32_t source_id = robFrag.rod_source_id();
        std::vector<IdentifierHash>::const_iterator it = coll.begin();
        to_return.reserve(coll.size());
        for ( ; it!= coll.end() ; ++it)
        {
            if( source_id == (m_rpcCabling->padHashFunction())->hash2source(*it) ) 
                to_return.push_back(IdentifierHash(*it));
        }
    }

    return to_return;
}
