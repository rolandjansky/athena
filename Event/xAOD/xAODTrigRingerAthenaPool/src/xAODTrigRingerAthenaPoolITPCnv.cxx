/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "AthenaKernel/TPCnvFactory.h"

#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "xAODTrigRinger/versions/TrigRingerRingsContainer_v1.h"

#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/versions/TrigRingerRings_v1.h"

#include "xAODTrigRinger/TrigRNNOutputContainer.h"
#include "xAODTrigRinger/versions/TrigRNNOutputContainer_v1.h"

#include "xAODTrigRinger/TrigRNNOutput.h"
#include "xAODTrigRinger/versions/TrigRNNOutput_v1.h"

// local includes
#include "xAODTrigRingerRingsContainerCnv_v1.h"
#include "xAODTrigRingerRingsAuxContainerCnv_v1.h"

#include "xAODTrigRNNOutputContainerCnv_v1.h"
#include "xAODTrigRNNOutputAuxContainerCnv_v1.h"

DECLARE_TPCNV_FACTORY( xAODTrigRingerRingsContainerCnv_v1,
                       xAOD::TrigRingerRingsContainer,
                       xAOD::TrigRingerRingsContainer_v1,
                       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODTrigRNNOutputContainerCnv_v1,
                       xAOD::TrigRNNOutputContainer,
                       xAOD::TrigRNNOutputContainer_v1,
                       Athena::TPCnvVers::Old )
