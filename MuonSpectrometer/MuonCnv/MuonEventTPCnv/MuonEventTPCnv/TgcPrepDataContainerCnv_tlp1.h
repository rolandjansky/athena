/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCPREPDATACONTAINERCNV_TLP1_H
#define TGCPREPDATACONTAINERCNV_TLP1_H


#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TgcPrepDataContainer_tlp1.h"

#include "MuonEventTPCnv/MuonPrepRawData/TgcPrepDataContainerCnv_p1.h"
#include "MuonEventTPCnv/MuonPrepRawData/TgcPrepDataCnv_p1.h"
#include "TrkEventTPCnv/TrkPrepRawData/PrepRawDataCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"

#include <iostream>

class TgcPrepDataContainerCnv_tlp1
   : public AthenaPoolTopLevelTPConverter<Muon::TgcPrepDataContainerCnv_p1, Muon::TgcPrepDataContainer_tlp1 >
{
public:

  TgcPrepDataContainerCnv_tlp1();
  virtual ~TgcPrepDataContainerCnv_tlp1() {}

  virtual void	setPStorage( Muon::TgcPrepDataContainer_tlp1 *storage );
// return list of tokens to extending persistent objects
// it is stored in m_tokenList member of the Track persistent rep
  virtual TPCnvTokenList_p1* 	getTokenListVar() { return &(getTLPersObject()->m_tokenList); }

protected:

  TgcPrepDataCnv_p1             m_muonPrdCnv;
  PrepRawDataCnv_p1             m_prepDataCnv;
  LocalPositionCnv_p1           m_locPosCnv;
  ErrorMatrixCnv_p1             m_errorMatCnv;
};


#endif


