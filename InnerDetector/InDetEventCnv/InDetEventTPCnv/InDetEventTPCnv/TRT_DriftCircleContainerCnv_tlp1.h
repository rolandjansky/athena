/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DRIFTCIRCLECONTAINERCNV_TLP1_H
#define TRT_DRIFTCIRCLECONTAINERCNV_TLP1_H


#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TRT_DriftCircleContainer_tlp1.h"

#include "InDetEventTPCnv/InDetPrepRawData/TRT_DriftCircleContainerCnv_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/TRT_DriftCircleCnv_p1.h"
#include "TrkEventTPCnv/TrkPrepRawData/PrepRawDataCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"


#include <iostream>

class TRT_DriftCircleContainerCnv_tlp1
   : public AthenaPoolTopLevelTPConverter<InDet::TRT_DriftCircleContainerCnv_p1, InDet::TRT_DriftCircleContainer_tlp1 >
{
public:

  TRT_DriftCircleContainerCnv_tlp1();
  virtual ~TRT_DriftCircleContainerCnv_tlp1() {}

  virtual void	setPStorage( InDet::TRT_DriftCircleContainer_tlp1 *storage );
// return list of tokens to extending persistent objects
// it is stored in m_tokenList member of the Track persistent rep
  virtual TPCnvTokenList_p1* 	getTokenListVar() { return &(getTLPersObject()->m_tokenList); }

protected:

  TRT_DriftCircleCnv_p1		m_trtDriftCirclesCnv;
  PrepRawDataCnv_p1             m_prepDataCnv;
  LocalPositionCnv_p1           m_locPosCnv;
  ErrorMatrixCnv_p1             m_errorMatCnv;


};


#endif


