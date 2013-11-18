/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRIGCOINDATA_RPCCOINDATACONTAINER_H
#define MUONTRIGCOINDATA_RPCCOINDATACONTAINER_H

#include "MuonTrigCoinData/MuonCoinDataContainer.h"
#include "MuonTrigCoinData/RpcCoinDataCollection.h"
#include "CLIDSvc/CLASS_DEF.h"

namespace Muon {
    
typedef MuonCoinDataContainer< RpcCoinDataCollection > RpcCoinDataContainer;

}

CLASS_DEF( Muon::RpcCoinDataContainer , 1190485326 , 1 )

#endif
