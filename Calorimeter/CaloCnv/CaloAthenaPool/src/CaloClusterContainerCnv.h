/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef CALOATHENAPOOL_CALOCLUSTERCONTAINERCNV_H
#define CALOATHENAPOOL_CALOCLUSTERCONTAINERCNV_H

#include "CaloTPCnv/CaloClusterContainerCnv_p1.h"
#include "CaloTPCnv/CaloClusterContainerCnv_p2.h"
#include "CaloTPCnv/CaloClusterContainerCnv_p3.h"
#include "CaloTPCnv/CaloClusterContainerCnv_p4.h"
#include "CaloTPCnv/CaloClusterContainerCnv_p5.h"
#include "CaloTPCnv/CaloClusterContainerCnv_p6.h"
#include "CaloTPCnv/CaloClusterContainerCnv_p7.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloTPCnv/CaloClusterContainer_p1.h"
#include "CaloTPCnv/CaloClusterContainer_p2.h"
#include "CaloTPCnv/CaloClusterContainer_p3.h"
#include "CaloTPCnv/CaloClusterContainer_p4.h"
#include "CaloTPCnv/CaloClusterContainer_p5.h"
#include "CaloTPCnv/CaloClusterContainer_p6.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"


typedef T_AthenaPoolTPCnvCnv<CaloClusterContainer,
                             CaloClusterContainerCnv_p7,
                             CaloClusterContainerCnv_p6,
                             CaloClusterContainerCnv_p5,
                             CaloClusterContainerCnv_p4,
                             CaloClusterContainerCnv_p3,
                             CaloClusterContainerCnv_p2,
                             CaloClusterContainerCnv_p1,
                             T_TPCnvNull<CaloClusterContainer> >
  CaloClusterContainerCnv;


#endif
