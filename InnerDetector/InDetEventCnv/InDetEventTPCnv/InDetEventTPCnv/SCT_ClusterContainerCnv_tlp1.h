/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CLUSTERCONTAINERCNV_TLP1_H
#define SCT_CLUSTERCONTAINERCNV_TLP1_H


#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "SCT_ClusterContainer_tlp1.h"

#include "InDetEventTPCnv/InDetPrepRawData/SCT_ClusterContainerCnv_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/SiClusterCnv_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/SCT_ClusterCnv_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/SiWidthCnv_p1.h"
#include "TrkEventTPCnv/TrkPrepRawData/PrepRawDataCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"


#include <iostream>

class SCT_ClusterContainerCnv_tlp1
   : public AthenaPoolTopLevelTPConverter<InDet::SCT_ClusterContainerCnv_p1, InDet::SCT_ClusterContainer_tlp1 >
{
public:

  SCT_ClusterContainerCnv_tlp1();
  virtual ~SCT_ClusterContainerCnv_tlp1() = default;

  virtual void	setPStorage( InDet::SCT_ClusterContainer_tlp1 *storage );
// return list of tokens to extending persistent objects
// it is stored in m_tokenList member of the Track persistent rep
  virtual TPCnvTokenList_p1* 	getTokenListVar() { return &(getTLPersObject()->m_tokenList); }

  void setUseDetectorElement(const bool useDetectorElement);

protected:

  SiClusterCnv_p1		m_siClusterCnv;
  SCT_ClusterCnv_p1		m_sctClusterCnv;
  SiWidthCnv_p1			m_siWidthCnv;
  PrepRawDataCnv_p1             m_prepDataCnv;
  LocalPositionCnv_p1           m_locPosCnv;
  ErrorMatrixCnv_p1             m_errorMatCnv;


};


#endif


