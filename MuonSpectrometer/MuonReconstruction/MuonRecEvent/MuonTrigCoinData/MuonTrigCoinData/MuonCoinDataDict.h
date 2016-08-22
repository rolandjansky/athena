/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOINDATADICT_H
#define MUONCOINDATADICT_H

#include "MuonTrigCoinData/MuonCoinDataContainer.h"
#include "MuonTrigCoinData/TgcCoinData.h"
#include "MuonTrigCoinData/TgcCoinDataContainer.h"
#include "MuonTrigCoinData/RpcCoinData.h"
#include "MuonTrigCoinData/RpcCoinDataContainer.h"

namespace tmpMuonTrigCoinData
{
  struct tmp 
  {
    DataVector<Muon::TgcCoinData> tmpDV1Tgc;
    DataVector<Muon::RpcCoinData> tmpDV1Rpc;
    Muon::TgcCoinDataContainer tmpCont1;
    
    DataVector<Muon::MuonCoinDataCollection<Muon::TgcCoinData> >    dummy1Tgc;
    std::vector<Muon::MuonCoinDataCollection<Muon::TgcCoinData>* >  dummy2Tgc;
    DataVector<Muon::MuonCoinDataCollection<Muon::RpcCoinData> >    dummy1Rpc;
    std::vector<Muon::MuonCoinDataCollection<Muon::RpcCoinData>* >  dummy2Rpc;
  };
}

#endif
