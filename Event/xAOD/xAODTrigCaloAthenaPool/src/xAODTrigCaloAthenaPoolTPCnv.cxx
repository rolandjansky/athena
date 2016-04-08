/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigCaloAthenaPoolTPCnv.cxx 713751 2015-12-09 20:24:00Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/TPCnvFactory.h"

// EDM include(s):
#include "xAODTrigCalo/CaloClusterTrigAuxContainer.h"
#include "xAODCaloEvent/versions/CaloClusterAuxContainer_v1.h"
#include "xAODCaloEvent/versions/CaloClusterAuxContainer_v2.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"
#include "xAODTrigCalo/versions/TrigEMClusterAuxContainer_v1.h"

// Local include(s):
#include "xAODCaloClusterTrigAuxContainerOfflineCnv_v1.h"
#include "xAODCaloClusterTrigAuxContainerOfflineCnv_v2.h"
#include "xAODTrigEMClusterAuxContainerCnv_v1.h"

// Declare the T/P converter(s):
DECLARE_TRIGTPCNV_FACTORY( xAODCaloClusterTrigAuxContainerOfflineCnv_v1,
                           xAOD::CaloClusterTrigAuxContainer,
                           xAOD::CaloClusterAuxContainer_v1,
                           Athena::TPCnvVers::Old )
DECLARE_TRIGTPCNV_FACTORY( xAODCaloClusterTrigAuxContainerOfflineCnv_v2,
                           xAOD::CaloClusterTrigAuxContainer,
                           xAOD::CaloClusterAuxContainer_v2,
                           Athena::TPCnvVers::Old )
DECLARE_TPCNV_FACTORY( xAODTrigEMClusterAuxContainerCnv_v1,
                       xAOD::TrigEMClusterAuxContainer,
                       xAOD::TrigEMClusterAuxContainer_v1,
                       Athena::TPCnvVers::Old )
