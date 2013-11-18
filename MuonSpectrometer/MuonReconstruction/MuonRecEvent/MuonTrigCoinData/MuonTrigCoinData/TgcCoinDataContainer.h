/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRIGCOINDATA_TGCCOINDATACONTAINER_H
#define MUONTRIGCOINDATA_TGCCOINDATACONTAINER_H

#include "MuonTrigCoinData/MuonCoinDataContainer.h"
#include "MuonTrigCoinData/TgcCoinDataCollection.h"
#include "CLIDSvc/CLASS_DEF.h"

namespace Muon {
    
typedef MuonCoinDataContainer< TgcCoinDataCollection > TgcCoinDataContainer;

}

CLASS_DEF( Muon::TgcCoinDataContainer , 1190485325 , 1 )

#endif
