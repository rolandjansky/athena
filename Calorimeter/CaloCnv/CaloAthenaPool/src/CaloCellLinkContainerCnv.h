/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOATHENAPOOL_CALOCELLLINKCONTAINERCNV_H
#define CALOATHENAPOOL_CALOCELLLINKCONTAINERCNV_H


#include "CaloTPCnv/CaloCellLinkContainerCnv_p1.h"
#include "CaloTPCnv/CaloCellLinkContainerCnv_p2.h"
#include "CaloTPCnv/CaloCellLinkContainer_p1.h"
#include "CaloTPCnv/CaloCellLinkContainer_p2.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"


typedef T_AthenaPoolTPCnvCnv<CaloCellLinkContainer,
                             CaloCellLinkContainerCnv_p2,
                             CaloCellLinkContainerCnv_p1,
                             T_TPCnvNull<CaloCellLinkContainer> >
  CaloCellLinkContainerCnv;


#endif
