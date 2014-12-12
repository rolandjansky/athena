/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloClusterAthenaPoolTPCnv.cxx 628099 2014-11-13 09:30:05Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/TPCnvFactory.h"

// EDM include(s):
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODCaloEvent/versions/CaloClusterAuxContainer_v1.h"

// Local include(s):
#include "xAODCaloClusterAuxContainerCnv_v1.h"

// Declare the T/P converter(s):
DECLARE_TPCNV_FACTORY( xAODCaloClusterAuxContainerCnv_v1,
                       xAOD::CaloClusterAuxContainer,
                       xAOD::CaloClusterAuxContainer_v1,
                       Athena::TPCnvVers::Old )


