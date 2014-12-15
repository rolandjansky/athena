/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Takashi Kubota - June 30, 2008 */
#ifndef TGCCOINDATACONTAINERCNV_TLP2_H
#define TGCCOINDATACONTAINERCNV_TLP2_H


#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TgcCoinDataContainer_tlp2.h"

#include "MuonEventTPCnv/MuonTrigCoinData/TgcCoinDataContainerCnv_p2.h"
#include "MuonEventTPCnv/MuonTrigCoinData/TgcCoinDataCnv_p2.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"

#include <iostream>

class TgcCoinDataContainerCnv_tlp2
   : public AthenaPoolTopLevelTPConverter<Muon::TgcCoinDataContainerCnv_p2, Muon::TgcCoinDataContainer_tlp2 >
{
public:

  TgcCoinDataContainerCnv_tlp2();
  virtual ~TgcCoinDataContainerCnv_tlp2() {}

  virtual void	setPStorage( Muon::TgcCoinDataContainer_tlp2 *storage );
// return list of tokens to extending persistent objects
// it is stored in m_tokenList member of the Track persistent rep
  virtual TPCnvTokenList_p1* 	getTokenListVar() { return &(getTLPersObject()->m_tokenList); }

protected:

  TgcCoinDataCnv_p2             m_tgcCoinDataCnv;
  LocalPositionCnv_p1           m_locPosCnv;
  ErrorMatrixCnv_p1             m_errorMatCnv;
};


#endif


