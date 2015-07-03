/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOINDATADICT_H
#define MUONCOINDATADICT_H

#include "MuonTrigCoinData/TgcCoinData.h"
#include "MuonTrigCoinData/MuonCoinDataContainer.h"
#include "MuonTrigCoinData/TgcCoinDataContainer.h"

namespace tmpMuonTrigCoinData
{
  struct tmp 
  {
    DataVector<Muon::TgcCoinData> tmpDV1;
    Muon::TgcCoinDataContainer tmpCont1;
    
    DataVector<Muon::MuonCoinDataCollection<Muon::TgcCoinData> >    dummy1;
    std::vector<Muon::MuonCoinDataCollection<Muon::TgcCoinData>* >  dummy2;
  };
}

#endif
