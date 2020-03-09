// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigEMClusterAuxContainerCnv_v1.h 619884 2014-10-03 15:00:40Z $
#ifndef XAODTRIGCALOATHENAPOOL_XAODTRIGEMCLUSTERAUXCONTAINERCNV_V1_H
#define XAODTRIGCALOATHENAPOOL_XAODTRIGEMCLUSTERAUXCONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AuxContainerCopyTPCnv.h"

// EDM include(s):
#include "xAODTrigCalo/versions/TrigEMClusterAuxContainer_v1.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"

/// Converter class used for reading xAOD::TrigEMClusterAuxContainer_v1
///
/// This converter implements the conversion from xAOD::TrigEMClusterAuxContainer_v1
/// to the latest version of the class. In a way that makes this converter
/// usable both from the POOL converter, and the BS converter.
///
typedef T_AuxContainerCopyTPCnv< xAOD::TrigEMClusterAuxContainer,
                                 xAOD::TrigEMClusterAuxContainer_v1 >
  xAODTrigEMClusterAuxContainerCnv_v1;


#endif // XAODTRIGCALOATHENAPOOL_XAODTRIGEMCLUSTERAUXCONTAINERCNV_V1_H
