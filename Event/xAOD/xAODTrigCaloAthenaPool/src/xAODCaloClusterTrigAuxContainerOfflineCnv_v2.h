// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloClusterTrigAuxContainerOfflineCnv_v2.h 705793 2015-11-04 13:13:26Z krasznaa $
#ifndef XAODTRIGCALOATHENAPOOL_XAODCALOCLUSTERTRIGAUXCONTAINEROFFLINECNV_V2_H
#define XAODTRIGCALOATHENAPOOL_XAODCALOCLUSTERTRIGAUXCONTAINEROFFLINECNV_V2_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AuxContainerCopyTPCnv.h"

// EDM include(s):
#include "xAODCaloEvent/versions/CaloClusterAuxContainer_v2.h"
#include "xAODTrigCalo/CaloClusterTrigAuxContainer.h"

/// Converter class used to read xAOD::CaloClusterAuxContainer_v2
///
/// Since we switched from using the offline xAOD::CaloClusterAuxContainer
/// type to using a trigger specific xAOD::CaloClusterTrigAuxContainer type
/// at the end of 2015, we now need to provide means of translating the offline
/// objects into the trigger ones automatically, to be able to read old
/// RAW files correctly.
///
typedef T_AuxContainerCopyTPCnv< xAOD::CaloClusterTrigAuxContainer,
                                 xAOD::CaloClusterAuxContainer_v2 >
  xAODCaloClusterTrigAuxContainerOfflineCnv_v2;


#endif // XAODTRIGCALOATHENAPOOL_XAODCALOCLUSTERTRIGAUXCONTAINEROFFLINECNV_V2_H
