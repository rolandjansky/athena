/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCLUSTERCONTAINERCNV_TLP1_H
#define PIXELCLUSTERCONTAINERCNV_TLP1_H


#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "PixelClusterContainer_tlp1.h"

#include "InDetEventTPCnv/InDetPrepRawData/PixelClusterContainerCnv_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/PixelClusterCnv_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/SiClusterCnv_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/SiWidthCnv_p1.h"
#include "TrkEventTPCnv/TrkPrepRawData/PrepRawDataCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"


#include <iostream>

class PixelClusterContainerCnv_tlp1
   : public AthenaPoolTopLevelTPConverter<InDet::PixelClusterContainerCnv_p1, InDet::PixelClusterContainer_tlp1 >
{
public:

  PixelClusterContainerCnv_tlp1();
  virtual ~PixelClusterContainerCnv_tlp1() {}

  virtual void	setPStorage( InDet::PixelClusterContainer_tlp1 *storage );
// return list of tokens to extending persistent objects
// it is stored in m_tokenList member of the Track persistent rep
  virtual TPCnvTokenList_p1* 	getTokenListVar() { return &(getTLPersObject()->m_tokenList); }

protected:

  PixelClusterCnv_p1		m_pixClusterCnv;
  SiClusterCnv_p1		m_siClusterCnv;
  SiWidthCnv_p1		m_siWidthCnv;
  PrepRawDataCnv_p1             m_prepDataCnv;
  LocalPositionCnv_p1           m_locPosCnv;
  ErrorMatrixCnv_p1             m_errorMatCnv;


};


#endif


