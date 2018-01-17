/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLTESTATHENAPOOL_PIXELRDOELEMLINKVECCNV_H
#define ATHENAPOOLTESTATHENAPOOL_PIXELRDOELEMLINKVECCNV_H


#include "PixelRDOElemLinkVecCnv_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"
#include "AthenaPoolTestData/PixelRDOElemLinkVec.h"


typedef T_AthenaPoolTPCnvCnv<PixelRDOElemLinkVec,PixelRDOElemLinkVecCnv_p1>
  PixelRDOElemLinkVecCnv;


#endif
