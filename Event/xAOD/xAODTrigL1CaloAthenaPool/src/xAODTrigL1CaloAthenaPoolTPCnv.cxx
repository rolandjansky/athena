/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigL1CaloAthenaPoolTPCnv.cxx 694352 2015-09-11 12:05:29Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/TPCnvFactory.h"

// EDM include(s):
#include "xAODTrigL1Calo/RODHeaderContainer.h"
#include "xAODTrigL1Calo/versions/RODHeaderContainer_v1.h"
#include "xAODTrigL1Calo/RODHeaderAuxContainer.h"
#include "xAODTrigL1Calo/versions/RODHeaderAuxContainer_v1.h"

// Local include(s):
#include "xAODRODHeaderContainerCnv_v1.h"
#include "xAODRODHeaderAuxContainerCnv_v1.h"

// Declare the T/P converter(s):
DECLARE_TPCNV_FACTORY( xAODRODHeaderContainerCnv_v1,
                       xAOD::RODHeaderContainer,
                       xAOD::RODHeaderContainer_v1,
                       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODRODHeaderAuxContainerCnv_v1,
                       xAOD::RODHeaderAuxContainer,
                       xAOD::RODHeaderAuxContainer_v1,
                       Athena::TPCnvVers::Old )
