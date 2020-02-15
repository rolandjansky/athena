/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDOTOPREPDATA_RPCRDOTOPREPDATATOOLMT_H
#define MUONRDOTOPREPDATA_RPCRDOTOPREPDATATOOLMT_H

#include "RpcRdoToPrepDataToolCore.h"
#include "MuonPrepRawData/MuonPrepDataCollection_Cache.h"
#include "MuonTrigCoinData/MuonTrigCoinData_Cache.h"

namespace Muon {

class RpcRdoToPrepDataToolMT : virtual public RpcRdoToPrepDataToolCore {
public:
  RpcRdoToPrepDataToolMT( const std::string&, const std::string&, const IInterface* );
  virtual ~RpcRdoToPrepDataToolMT();
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

protected:
  virtual StatusCode manageOutputContainers(bool& firstTimeInTheEvent) override;

private:
   /// This is the key for the cache for the RPC PRD containers, can be empty
   SG::UpdateHandleKey<RpcPrepDataCollection_Cache> m_prdContainerCacheKey ;
   SG::UpdateHandleKey<RpcCoinDataCollection_Cache> m_coinContainerCacheKey ;
};

}

#endif // !ATHEXJOBOPTIONS_CONCRETETOOL_H
