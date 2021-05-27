/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCOINDATACONTAINER_TLP4_H
#define TGCCOINDATACONTAINER_TLP4_H

//-----------------------------------------------------------------------------
// MuonTrigCoinData
//-----------------------------------------------------------------------------
#include "MuonEventTPCnv/MuonTrigCoinData/MuonCoinDataContainer_p1.h"
#include "MuonEventTPCnv/MuonTrigCoinData/TgcCoinData_p4.h"
#include "TrkEventTPCnv/TrkEventPrimitives/HepSymMatrix_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPosition_p1.h"


// Token
#include "AthenaPoolUtilities/TPCnvTokenList_p1.h"

namespace Muon
{
   class TgcCoinDataContainer_tlp4
   {
  public:
     TgcCoinDataContainer_tlp4() {}
     TPCnvTokenList_p1		                        m_tokenList;

// This is the basic info
     std::vector< Muon::MuonCoinDataContainer_p1>       m_pixDCCont;
     std::vector< Muon::TgcCoinData_p4 >                m_tgcCoinDatas;
// This are the base classes
// This is info held by the base class
     std::vector< Trk::LocalPosition_p1>                m_locPos;
     std::vector< Trk::HepSymMatrix_p1>                 m_errorMat;
   };
}


#endif
