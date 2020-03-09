// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloClusterTrigAuxContainerCnv.h
#ifndef XAODTRIGCALOATHENAPOOL_XAODCALOCLUSTERTRIGAUXCONTAINERCNV_H
#define XAODTRIGCALOATHENAPOOL_XAODCALOCLUSTERTRIGAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"
#include "AthenaPoolCnvSvc/T_AuxContainerCopyTPCnv.h"

// EDM include(s):
#include "xAODTrigCalo/CaloClusterTrigAuxContainer.h"
#include "xAODCaloClusterTrigAuxContainerOfflineCnv_v1.h"
#include "xAODCaloClusterTrigAuxContainerOfflineCnv_v2.h"


typedef T_AthenaPoolAuxContainerCnv< xAOD::CaloClusterTrigAuxContainer,
                                     xAODCaloClusterTrigAuxContainerOfflineCnv_v2,
                                     xAODCaloClusterTrigAuxContainerOfflineCnv_v1 >
   xAODCaloClusterTrigAuxContainerCnv;


#endif // XAODTRIGCALOATHENAPOOL_XAODCALOCLUSTERTRIGAUXCONTAINERCNV_H
