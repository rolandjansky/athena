/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  virtual StatusCode decode ( std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& selectedIdVect ) override;
  virtual StatusCode decode ( const std::vector<uint32_t>& robIds ) override;
  
protected:
  StatusCode manageOutputContainers(bool& firstTimeInTheEvent) override;
  StatusCode transferOutputToCache();
  void printMT();

private:
  /// This is the key for the cache for the MDT PRD containers, can be empty
  SG::UpdateHandleKey<RpcPrepDataCollection_Cache> m_prdContainerCacheKey ;
  SG::UpdateHandleKey<RpcCoinDataCollection_Cache> m_coindataContainerCacheKey ;
  /// As this code is complex, we will store access to the cache container in the MT tool
  /// and make contents available to Core code 
  Muon::RpcPrepDataContainer* m_rpcPrepDataContainerFromCache;
  Muon::RpcCoinDataContainer* m_rpcCoinDataContainerFromCache;


};

}

#endif // !ATHEXJOBOPTIONS_CONCRETETOOL_H
