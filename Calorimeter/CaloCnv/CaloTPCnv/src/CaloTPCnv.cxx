/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "CaloTPCnv/CaloClusterContainerCnv_p2.h"
#include "CaloTPCnv/CaloClusterContainerCnv_p3.h"
#include "CaloTPCnv/CaloClusterContainerCnv_p4.h"
#include "CaloTPCnv/CaloClusterContainerCnv_p5.h"
#include "CaloTPCnv/CaloClusterContainerCnv_p6.h"
#include "CaloTPCnv/CaloClusterContainerCnv_p7.h"

DECLARE_TPCNV_FACTORY(CaloClusterContainerCnv_p2,
                      CaloClusterContainer,
                      CaloClusterContainer_p2,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(CaloClusterContainerCnv_p3,
                      CaloClusterContainer,
                      CaloClusterContainer_p3,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(CaloClusterContainerCnv_p4,
                      CaloClusterContainer,
                      CaloClusterContainer_p4,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(CaloClusterContainerCnv_p5,
                      CaloClusterContainer,
                      CaloClusterContainer_p5,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(CaloClusterContainerCnv_p6,
                      CaloClusterContainer,
                      CaloClusterContainer_p6,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(CaloClusterContainerCnv_p7,
                      CaloClusterContainer,
                      CaloClusterContainer_p7,
                      Athena::TPCnvVers::Current)
                      

#include "CaloTPCnv/CaloCellLinkContainerCnv_p1.h"
#include "CaloTPCnv/CaloCellLinkContainerCnv_p2.h"

DECLARE_TPCNV_FACTORY(CaloCellLinkContainerCnv_p1,
                      CaloCellLinkContainer,
                      CaloCellLinkContainer_p1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(CaloCellLinkContainerCnv_p2,
                      CaloCellLinkContainer,
                      CaloCellLinkContainer_p2,
                      Athena::TPCnvVers::Current)


#include "CaloTPCnv/CaloTopoTowerContainerCnv_p1.h"
DECLARE_TPCNV_FACTORY(CaloTopoTowerContainerCnv_p1,
                      CaloTopoTowerContainer,
                      CaloTopoTowerContainer_p1,
                      Athena::TPCnvVers::Current)

#include "CaloTPCnv/CaloTowerContainerCnv_p1.h"
DECLARE_TPCNV_FACTORY(CaloTowerContainerCnv_p1,
                      CaloTowerContainer,
                      CaloTowerContainer_p1,
                      Athena::TPCnvVers::Current)

#include "CaloTPCnv/CaloShowerContainerCnv_p1.h"
#include "CaloTPCnv/CaloShowerContainerCnv_p2.h"

DECLARE_TPCNV_FACTORY(CaloShowerContainerCnv_p1,
                      CaloShowerContainer,
                      CaloShowerContainer_p1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(CaloShowerContainerCnv_p2,
                      CaloShowerContainer,
                      CaloShowerContainer_p2,
                      Athena::TPCnvVers::Current)

#include "CaloTPCnv/CaloCellContainerCnv_p1.h"

DECLARE_TPCNV_FACTORY(CaloCellContainerCnv_p1,
                      CaloCellContainer,
                      CaloCompactCellContainer,
                      Athena::TPCnvVers::Current)
