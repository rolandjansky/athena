/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Takashi Kubota - June 30, 2008 */
#ifndef MUONCOINDATACONTAINER_P1_H
#define MUONCOINDATACONTAINER_P1_H

#include <vector>
#include <string>
#include "MuonEventTPCnv/MuonTrigCoinData/MuonCoinDataCollection_p1.h"
#include "AthenaPoolUtilities/TPObjRef.h"

namespace Muon {
class MuonCoinDataContainer_p1   
{
 public:
/// Default constructor
  MuonCoinDataContainer_p1 ();
  //private:
  std::vector<Muon::MuonCoinDataCollection_p1>  m_collections;
  std::vector<TPObjRef>                         m_CoinData;
};

} 


// inlines
 
inline Muon::MuonCoinDataContainer_p1::MuonCoinDataContainer_p1() {}
 
#endif
