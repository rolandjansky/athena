/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODHIEventAthenaPoolTPCnv.cxx 693658 2015-09-08 10:00:53Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/TPCnvFactory.h"

// EDM include(s):
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODHIEvent/versions/HIEventShapeContainer_v1.h"
#include "xAODHIEvent/HIEventShapeAuxContainer.h"
#include "xAODHIEvent/versions/HIEventShapeAuxContainer_v1.h"

// Local include(s):
#include "xAODHIEventShapeContainerCnv_v1.h"
#include "xAODHIEventShapeAuxContainerCnv_v1.h"

// Declare the T/P converter(s):
DECLARE_TPCNV_FACTORY( xAODHIEventShapeContainerCnv_v1,
                       xAOD::HIEventShapeContainer,
                       xAOD::HIEventShapeContainer_v1,
                       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODHIEventShapeAuxContainerCnv_v1,
                       xAOD::HIEventShapeAuxContainer,
                       xAOD::HIEventShapeAuxContainer_v1,
                       Athena::TPCnvVers::Old )
