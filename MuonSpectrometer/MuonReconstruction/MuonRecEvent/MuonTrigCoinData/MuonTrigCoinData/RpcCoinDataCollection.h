/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRIGCOINDATA_RPCCOINDATACOLLECTION_H
#define MUONTRIGCOINDATA_RPCCOINDATACOLLECTION_H

#include "MuonTrigCoinData/RpcCoinData.h"
#include "MuonTrigCoinData/MuonCoinDataCollection.h"
#include "CLIDSvc/CLASS_DEF.h"

namespace Muon {
typedef MuonCoinDataCollection< RpcCoinData > RpcCoinDataCollection;
}
    /**Overload of << operator for MsgStream for debug output*/
    MsgStream& operator << ( MsgStream& sl, const Muon::RpcCoinDataCollection & coll);
    
    /**Overload of << operator for std::ostream for debug output*/ 
    std::ostream& operator << ( std::ostream& sl, const Muon::RpcCoinDataCollection& coll);
    

CLASS_DEF(Muon::RpcCoinDataCollection, 1216812642, 1 )


#endif
