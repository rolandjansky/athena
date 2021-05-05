/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDOTOPREPDATA_RPCRDOTOPREPDATATOOLMT_H
#define MUONRDOTOPREPDATA_RPCRDOTOPREPDATATOOLMT_H

#include "RpcRdoToPrepDataToolCore.h"
#include "MuonPrepRawData/MuonPrepDataCollection_Cache.h"
#include "MuonTrigCoinData/MuonTrigCoinData_Cache.h"

namespace Muon {

class RpcRdoToPrepDataToolMT
  : public extends<RpcRdoToPrepDataToolCore, IMuonRdoToPrepDataTool>
{
public:
  RpcRdoToPrepDataToolMT( const std::string&, const std::string&, const IInterface* );
  virtual ~RpcRdoToPrepDataToolMT();
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode decode ( std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& selectedIdVect ) const override;
  virtual StatusCode decode ( const std::vector<uint32_t>& robIds ) const override;
  
  virtual void printPrepData() const override;

protected:
  StatusCode transferAndRecordPrepData (Muon::RpcPrepDataContainer& localContainer) const;
  StatusCode transferAndRecordCoinData (Muon::RpcCoinDataContainer& localContainer) const;
  void printMTPrepData (Muon::RpcPrepDataContainer& prepData) const;
  void printMTCoinData (Muon::RpcCoinDataContainer& prepData) const;


private:
  class MyState
    : public State
  {
  public:
    MyState (const RpcIdHelper& idHelper, MsgStream& msg);

    Muon::RpcPrepDataContainer m_localPrepData;
    Muon::RpcCoinDataContainer m_localCoinData;
    getPrepCollection_func m_getPrepCollection;
    getCoinCollection_func m_getCoinCollection;

    std::unordered_map<Identifier, Muon::RpcPrepDataCollection*> m_rpcPrepDataCollections;
    std::unordered_map<Identifier, Muon::RpcCoinDataCollection*> m_rpcCoinDataCollections;
  };

  /// This is the key for the cache for the MDT PRD containers, can be empty
  SG::UpdateHandleKey<RpcPrepDataCollection_Cache> m_prdContainerCacheKey ;
  SG::UpdateHandleKey<RpcCoinDataCollection_Cache> m_coindataContainerCacheKey ;
};

}

#endif // !ATHEXJOBOPTIONS_CONCRETETOOL_H
