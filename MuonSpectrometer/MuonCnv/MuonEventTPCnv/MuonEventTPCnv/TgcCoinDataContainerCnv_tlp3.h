/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCOINDATACONTAINERCNV_TLP3_H
#define TGCCOINDATACONTAINERCNV_TLP3_H


#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TgcCoinDataContainer_tlp3.h"

#include "MuonEventTPCnv/MuonTrigCoinData/TgcCoinDataContainerCnv_p3.h"
#include "MuonEventTPCnv/MuonTrigCoinData/TgcCoinDataCnv_p3.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"

#include <iostream>

class TgcCoinDataContainerCnv_tlp3
   : public AthenaPoolTopLevelTPConverter<Muon::TgcCoinDataContainerCnv_p3, Muon::TgcCoinDataContainer_tlp3 >
{
public:

  TgcCoinDataContainerCnv_tlp3();
  virtual ~TgcCoinDataContainerCnv_tlp3() {}

  virtual void	setPStorage( Muon::TgcCoinDataContainer_tlp3 *storage );
// return list of tokens to extending persistent objects
// it is stored in m_tokenList member of the Track persistent rep
  virtual TPCnvTokenList_p1* 	getTokenListVar() { return &(getTLPersObject()->m_tokenList); }

protected:

  TgcCoinDataCnv_p3             m_tgcCoinDataCnv;
  LocalPositionCnv_p1           m_locPosCnv;
  ErrorMatrixCnv_p1             m_errorMatCnv;
};


#endif


