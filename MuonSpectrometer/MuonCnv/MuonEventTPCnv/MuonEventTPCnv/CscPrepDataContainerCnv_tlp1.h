/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCPREPDATACONTAINERCNV_TLP1_H
#define CSCPREPDATACONTAINERCNV_TLP1_H


#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "CscPrepDataContainer_tlp1.h"

#include "MuonEventTPCnv/MuonPrepRawData/CscPrepDataContainerCnv_p1.h"
#include "MuonEventTPCnv/MuonPrepRawData/CscPrepDataCnv_p1.h"
#include "TrkEventTPCnv/TrkPrepRawData/PrepRawDataCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"

#include <iostream>

class CscPrepDataContainerCnv_tlp1
   : public AthenaPoolTopLevelTPConverter<Muon::CscPrepDataContainerCnv_p1, Muon::CscPrepDataContainer_tlp1 >
{
public:

  CscPrepDataContainerCnv_tlp1();
  virtual ~CscPrepDataContainerCnv_tlp1() {}

  virtual void	setPStorage( Muon::CscPrepDataContainer_tlp1 *storage );
// return list of tokens to extending persistent objects
// it is stored in m_tokenList member of the Track persistent rep
  virtual TPCnvTokenList_p1* 	getTokenListVar() { return &(getTLPersObject()->m_tokenList); }

protected:

  CscPrepDataCnv_p1             m_muonPrdCnv;
  PrepRawDataCnv_p1             m_prepDataCnv;
  LocalPositionCnv_p1           m_locPosCnv;
  ErrorMatrixCnv_p1             m_errorMatCnv;
};


#endif


