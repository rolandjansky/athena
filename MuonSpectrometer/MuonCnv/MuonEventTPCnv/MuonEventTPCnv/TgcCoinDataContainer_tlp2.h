/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Takashi Kubota - June 30, 2008 */
#ifndef TGCCOINDATACONTAINER_TLP2_H
#define TGCCOINDATACONTAINER_TLP2_H

//-----------------------------------------------------------------------------
// MuonTrigCoinData
//-----------------------------------------------------------------------------
#include "MuonEventTPCnv/MuonTrigCoinData/MuonCoinDataContainer_p1.h"
#include "MuonEventTPCnv/MuonTrigCoinData/TgcCoinData_p2.h"
#include "TrkEventTPCnv/TrkEventPrimitives/HepSymMatrix_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPosition_p1.h"


// Token
//#include "AthenaPoolUtilities/TPCnvTokenList_p1.h"

namespace Muon
{
   class TgcCoinDataContainer_tlp2
   {
  public:
     TgcCoinDataContainer_tlp2() {}
     TPCnvTokenList_p1		                        m_tokenList;

// This is the basic info
     std::vector< Muon::MuonCoinDataContainer_p1>       m_pixDCCont;
     std::vector< Muon::TgcCoinData_p2 >                m_tgcCoinDatas;
// This are the base classes
// This is info held by the base class
     std::vector< Trk::LocalPosition_p1>                m_locPos;
     std::vector< Trk::HepSymMatrix_p1>                 m_errorMat;
   };
}


#endif
