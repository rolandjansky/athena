/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Takashi Kubota - June 30, 2008 */
#ifndef TGCCOINDATACONTAINERCNV_TLP1_H
#define TGCCOINDATACONTAINERCNV_TLP1_H


#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TgcCoinDataContainer_tlp1.h"

#include "MuonEventTPCnv/MuonTrigCoinData/TgcCoinDataContainerCnv_p1.h"
#include "MuonEventTPCnv/MuonTrigCoinData/TgcCoinDataCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"

#include <iostream>

class TgcCoinDataContainerCnv_tlp1
   : public AthenaPoolTopLevelTPConverter<Muon::TgcCoinDataContainerCnv_p1, Muon::TgcCoinDataContainer_tlp1 >
{
public:

  TgcCoinDataContainerCnv_tlp1();
  virtual ~TgcCoinDataContainerCnv_tlp1() {}

  virtual void	setPStorage( Muon::TgcCoinDataContainer_tlp1 *storage );
// return list of tokens to extending persistent objects
// it is stored in m_tokenList member of the Track persistent rep
  virtual TPCnvTokenList_p1* 	getTokenListVar() { return &(getTLPersObject()->m_tokenList); }

protected:
  // 
  TgcCoinDataCnv_p1             m_tgcCoinDataCnv;
  LocalPositionCnv_p1           m_locPosCnv;
  ErrorMatrixCnv_p1             m_errorMatCnv;
};


#endif


