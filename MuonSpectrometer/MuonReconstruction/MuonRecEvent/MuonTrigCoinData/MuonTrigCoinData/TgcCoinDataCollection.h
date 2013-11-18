/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRIGCOINDATA_TGCCOINDATACOLLECTION_H
#define MUONTRIGCOINDATA_TGCCOINDATACOLLECTION_H

#include "MuonTrigCoinData/TgcCoinData.h"
#include "MuonTrigCoinData/MuonCoinDataCollection.h"
#include "CLIDSvc/CLASS_DEF.h"

namespace Muon {

typedef MuonCoinDataCollection< TgcCoinData > TgcCoinDataCollection;

    /**Overload of << operator for MsgStream for debug output*/
    MsgStream& operator << ( MsgStream& sl, const TgcCoinDataCollection& coll);
    
    /**Overload of << operator for std::ostream for debug output*/ 
    std::ostream& operator << ( std::ostream& sl, const TgcCoinDataCollection& coll);
    
}

CLASS_DEF(Muon::TgcCoinDataCollection, 1216812641, 1 )


#endif
