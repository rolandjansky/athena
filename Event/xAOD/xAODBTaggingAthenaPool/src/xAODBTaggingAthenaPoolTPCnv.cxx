/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODBTaggingAthenaPoolTPCnv.cxx 705820 2015-11-04 14:54:05Z krasznaa $

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
DECLARE_TPCNV_FACTORY( xAODBTaggingTrigAuxContainerOfflineCnv_v1,
                       xAOD::BTaggingTrigAuxContainer,
                       xAOD::BTaggingAuxContainer_v1,
                       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODBTaggingAuxContainerCnv_v1,
                       xAOD::BTaggingAuxContainer,
                       xAOD::BTaggingAuxContainer_v1,
                       Athena::TPCnvVers::Old )
