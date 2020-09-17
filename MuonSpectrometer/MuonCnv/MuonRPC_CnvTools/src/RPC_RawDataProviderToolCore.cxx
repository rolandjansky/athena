/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "RPC_RawDataProviderToolCore.h"

#include "MuonRPC_CnvTools/IRpcROD_Decoder.h"
#include "eformat/SourceIdentifier.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;
using eformat::helper::SourceIdentifier;

Muon::RPC_RawDataProviderToolCore::RPC_RawDataProviderToolCore(
    const std::string& t,
    const std::string& n,
    const IInterface*  p) :
    AthAlgTool(t, n, p),
    m_robDataProvider ("ROBDataProviderSvc",n) {
}

StatusCode Muon::RPC_RawDataProviderToolCore::initialize()
{
  // retrieve ROD decoder
  ATH_CHECK(m_decoder.retrieve());

  ATH_CHECK( m_containerKey.initialize() );
  ATH_CHECK( m_sec.initialize() );

  // get ROB data provider service
  ATH_CHECK(m_robDataProvider.retrieve());

  ATH_CHECK(m_readKey.initialize());

  return StatusCode::SUCCESS;

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
    SG::ReadCondHandle<RpcCablingCondData> readHandle{m_readKey};
    const RpcCablingCondData* readCdo{*readHandle};

    std::vector<IdentifierHash> to_return;
    if(coll.empty())
    {
    // get SubdetectorId and ModuleId
        uint32_t source_id = robFrag.rod_source_id();
        SourceIdentifier sid(source_id);
        to_return = (readCdo->rod2hash(sid.subdetector_id(), sid.module_id()));
    }
    else
    {
        uint32_t source_id = robFrag.rod_source_id();
        std::vector<IdentifierHash>::const_iterator it = coll.begin();
        to_return.reserve(coll.size());
        for ( ; it!= coll.end() ; ++it)
        {
          if( source_id == readCdo->hash2source(*it) ) to_return.push_back(IdentifierHash(*it));
        }
    }

    return to_return;
}
