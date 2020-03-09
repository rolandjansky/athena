// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterCellLinkContainerCnv.h 781569 2016-11-01 12:16:14Z wlampl $
#ifndef CALOCLUSTERCELLLINKCONTAINERCNV_H
#define CALOCLUSTERCELLLINKCONTAINERCNV_H

#include "CaloTPCnv/CaloClusterCellLinkContainerCnv_p1.h"
#include "CaloEvent/CaloClusterCellLinkContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"


typedef T_AthenaPoolTPCnvCnv<CaloClusterCellLinkContainer,
                             CaloClusterCellLinkContainerCnv_p1,
                             T_TPCnvNull<CaloClusterCellLinkContainer> >
  CaloClusterCellLinkContainerCnv;


#endif 
