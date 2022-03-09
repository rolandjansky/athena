/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "AthenaKernel/TPCnvFactory.h"

// EDM include(s):
#include "xAODBTagging/BTaggingTrigAuxContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBTagging/versions/BTaggingAuxContainer_v1.h"

// Local include(s):
#include "xAODBTaggingTrigAuxContainerOfflineCnv_v1.h"
#include "xAODBTaggingAuxContainerCnv.h"

// Declare the T/P converter(s):

/* March 2022: obsolete trigger-specific converter removed.
   see https://gitlab.cern.ch/atlas/athena/-/merge_requests/50342#note_5333334
DECLARE_TPCNV_FACTORY( xAODBTaggingTrigAuxContainerOfflineCnv_v1,
                       xAOD::BTaggingTrigAuxContainer,
                       xAOD::BTaggingAuxContainer_v1,
                       Athena::TPCnvVers::Old ) */

DECLARE_TPCNV_FACTORY( xAODBTaggingAuxContainerCnv_v1,
                       xAOD::BTaggingAuxContainer,
                       xAOD::BTaggingAuxContainer_v1,
                       Athena::TPCnvVers::Old )
