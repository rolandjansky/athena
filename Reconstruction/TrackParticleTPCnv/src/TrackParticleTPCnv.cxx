/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

//-----------------------------------------------------------------------------
#include "TrackParticleTPCnv/TrackParticleContainer_tlp1.h"
#include "TrackParticleTPCnv/TrackParticleContainerCnv_tlp1.h"

#include "TrackParticleTPCnv/TrackParticleContainer_tlp2.h"
#include "TrackParticleTPCnv/TrackParticleContainerCnv_tlp2.h"

#include "TrackParticleTPCnv/TrackParticleContainer_tlp3.h"
#include "TrackParticleTPCnv/TrackParticleContainerCnv_tlp3.h"

DECLARE_TPCNV_FACTORY(TrackParticleContainerCnv_tlp1,
                      Rec::TrackParticleContainer,
                      Rec::TrackParticleContainer_tlp1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_ARATPCNV_FACTORY(TrackParticleContainerARACnv_tlp1,
                      Rec::TrackParticleContainer,
                      Rec::TrackParticleContainer_tlp1,
                      Athena::TPCnvVers::Old)
                      
//----------------
DECLARE_TPCNV_FACTORY(TrackParticleContainerCnv_tlp2,
                      Rec::TrackParticleContainer,
                      Rec::TrackParticleContainer_tlp2,
                      Athena::TPCnvVers::Old)
                      
DECLARE_ARATPCNV_FACTORY(TrackParticleContainerARACnv_tlp2,
                      Rec::TrackParticleContainer,
                      Rec::TrackParticleContainer_tlp2,
                      Athena::TPCnvVers::Old)
                      
//----------------

DECLARE_TPCNV_FACTORY(TrackParticleContainerCnv_tlp3,
                      Rec::TrackParticleContainer,
                      Rec::TrackParticleContainer_tlp3,
                      Athena::TPCnvVers::Current)
                      
DECLARE_ARATPCNV_FACTORY(TrackParticleContainerARACnv_tlp3,
                      Rec::TrackParticleContainer,
                      Rec::TrackParticleContainer_tlp3,
                      Athena::TPCnvVers::Current)

