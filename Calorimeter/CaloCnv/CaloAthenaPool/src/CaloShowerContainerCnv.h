/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*- 
#ifndef CALOATHENAPOOL_CALOSHOWERCONTAINERCNV_H
#define CALOATHENAPOOL_CALOSHOWERCONTAINERCNV_H

#include "CaloTPCnv/CaloShowerContainerCnv_p1.h"
#include "CaloTPCnv/CaloShowerContainerCnv_p2.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"


typedef T_AthenaPoolTPCnvCnv<CaloShowerContainer,
                             CaloShowerContainerCnv_p2,
                             CaloShowerContainerCnv_p1,
                             T_TPCnvNull<CaloShowerContainer> >
  CaloShowerContainerCnv;


#endif
