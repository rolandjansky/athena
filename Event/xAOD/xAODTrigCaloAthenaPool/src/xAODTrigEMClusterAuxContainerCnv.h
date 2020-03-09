// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigEMClusterAuxContainerCnv.h 711098 2015-11-27 09:37:40Z krasznaa $
#ifndef XAODTRIGCALOATHENAPOOL_XAODTRIGEMCLUSTERAUXCONTAINERCNV_H
#define XAODTRIGCALOATHENAPOOL_XAODTRIGEMCLUSTERAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"
#include "AthenaPoolCnvSvc/T_AuxContainerCopyTPCnv.h"


// EDM include(s):
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"
#include "xAODTrigCalo/versions/TrigEMClusterAuxContainer_v1.h"


typedef T_AuxContainerCopyTPCnv< xAOD::TrigEMClusterAuxContainer,
                                 xAOD::TrigEMClusterAuxContainer_v1 >
  xAODTrigEMClusterAuxContainerCnv_v1;

typedef T_AthenaPoolAuxContainerCnv< xAOD::TrigEMClusterAuxContainer,
                                     xAODTrigEMClusterAuxContainerCnv_v1 >
   xAODTrigEMClusterAuxContainerCnv;


#endif // XAODTRIGCALOATHENAPOOL_XAODTRIGEMCLUSTERAUXCONTAINERCNV_H
