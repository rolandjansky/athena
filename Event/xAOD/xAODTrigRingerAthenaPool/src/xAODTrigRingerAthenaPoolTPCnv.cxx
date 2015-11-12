/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigRingerAthenaPoolTPCnv.cxx 707590 2015-11-12 19:09:03Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/TPCnvFactory.h"

// EDM include(s):
#include "xAODTrigRinger/TrigRNNOutputContainer.h"
#include "xAODTrigRinger/versions/TrigRNNOutputContainer_v1.h"
#include "xAODTrigRinger/TrigRNNOutputAuxContainer.h"
#include "xAODTrigRinger/versions/TrigRNNOutputAuxContainer_v1.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "xAODTrigRinger/versions/TrigRingerRingsContainer_v1.h"
#include "xAODTrigRinger/TrigRingerRingsAuxContainer.h"
#include "xAODTrigRinger/versions/TrigRingerRingsAuxContainer_v1.h"

// Local include(s):
#include "xAODTrigRNNOutputContainerCnv_v1.h"
#include "xAODTrigRNNOutputAuxContainerCnv_v1.h"
#include "xAODTrigRingerRingsContainerCnv_v1.h"
#include "xAODTrigRingerRingsAuxContainerCnv_v1.h"

// Declare the T/P converter(s):
DECLARE_TPCNV_FACTORY( xAODTrigRNNOutputContainerCnv_v1,
                       DataVector<xAOD::TrigRNNOutput>,
                       DataVector<xAOD::TrigRNNOutput_v1>,
                       Athena::TPCnvVers::Old )
DECLARE_TPCNV_FACTORY( xAODTrigRNNOutputAuxContainerCnv_v1,
                       xAOD::TrigRNNOutputAuxContainer,
                       xAOD::TrigRNNOutputAuxContainer_v1,
                       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODTrigRingerRingsContainerCnv_v1,
                       DataVector<xAOD::TrigRingerRings>,
                       DataVector<xAOD::TrigRingerRings_v1>,
                       Athena::TPCnvVers::Old )
DECLARE_TPCNV_FACTORY( xAODTrigRingerRingsAuxContainerCnv_v1,
                       xAOD::TrigRingerRingsAuxContainer,
                       xAOD::TrigRingerRingsAuxContainer_v1,
                       Athena::TPCnvVers::Old )
