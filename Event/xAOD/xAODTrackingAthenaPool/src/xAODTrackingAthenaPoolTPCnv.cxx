/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Gaudi/Athena include(s):
#include "AthenaKernel/TPCnvFactory.h"

// EDM include(s):
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/versions/TrackParticleAuxContainer_v1.h"
#include "xAODTracking/versions/TrackParticleAuxContainer_v2.h"
#include "xAODTracking/versions/TrackParticleAuxContainer_v3.h"
#include "xAODTracking/versions/TrackParticleAuxContainer_v4.h"

// Local include(s):
#include "xAODTrackParticleAuxContainerCnv_v1.h"
#include "xAODTrackParticleAuxContainerCnv_v2.h"
#include "xAODTrackParticleAuxContainerCnv_v3.h"
#include "xAODTrackParticleAuxContainerCnv_v4.h"


// Declare the T/P converter(s):
DECLARE_TPCNV_FACTORY( xAODTrackParticleAuxContainerCnv_v1,
                       xAOD::TrackParticleAuxContainer,
                       xAOD::TrackParticleAuxContainer_v1,
                       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODTrackParticleAuxContainerCnv_v2,
                       xAOD::TrackParticleAuxContainer,
                       xAOD::TrackParticleAuxContainer_v2,
                       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODTrackParticleAuxContainerCnv_v3,
                       xAOD::TrackParticleAuxContainer,
                       xAOD::TrackParticleAuxContainer_v3,
                       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODTrackParticleAuxContainerCnv_v4,
                       xAOD::TrackParticleAuxContainer,
                       xAOD::TrackParticleAuxContainer_v4,
                       Athena::TPCnvVers::Old )
