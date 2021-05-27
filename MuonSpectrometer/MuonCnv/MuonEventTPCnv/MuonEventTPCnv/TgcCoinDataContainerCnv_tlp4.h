/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCOINDATACONTAINERCNV_TLP4_H
#define TGCCOINDATACONTAINERCNV_TLP4_H


#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TgcCoinDataContainer_tlp4.h"

#include "MuonEventTPCnv/MuonTrigCoinData/TgcCoinDataContainerCnv_p4.h"
#include "MuonEventTPCnv/MuonTrigCoinData/TgcCoinDataCnv_p4.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"

#include <iostream>

class TgcCoinDataContainerCnv_tlp4
   : public AthenaPoolTopLevelTPConverter<Muon::TgcCoinDataContainerCnv_p4, Muon::TgcCoinDataContainer_tlp4 >
{
public:

  TgcCoinDataContainerCnv_tlp4();
  virtual ~TgcCoinDataContainerCnv_tlp4() {}

  virtual void	setPStorage( Muon::TgcCoinDataContainer_tlp4 *storage );
// return list of tokens to extending persistent objects
// it is stored in m_tokenList member of the Track persistent rep
  virtual TPCnvTokenList_p1* 	getTokenListVar() { return &(getTLPersObject()->m_tokenList); }

protected:

  TgcCoinDataCnv_p4             m_tgcCoinDataCnv;
  LocalPositionCnv_p1           m_locPosCnv;
  ErrorMatrixCnv_p1             m_errorMatCnv;
};


#endif


