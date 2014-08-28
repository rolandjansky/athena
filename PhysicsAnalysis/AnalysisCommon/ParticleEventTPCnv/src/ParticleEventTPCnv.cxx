/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

// converters
#include "ParticleEventTPCnv/CompositeParticleCnv_p1.h"
#include "ParticleEventTPCnv/CompositeParticleContainerCnv_p1.h"
#include "ParticleEventTPCnv/NeutrinoCnv_p1.h"
#include "ParticleEventTPCnv/NeutrinoCnv_p2.h"
#include "ParticleEventTPCnv/NeutrinoContainerCnv_p1.h"
#include "ParticleEventTPCnv/NeutrinoContainerCnv_p2.h"
#include "ParticleEventTPCnv/ParticleBaseCnv_p1.h"
#include "ParticleEventTPCnv/ParticleBaseCnv_p2.h"
#include "ParticleEventTPCnv/ParticleBaseLinksCnv_p1.h"
#include "ParticleEventTPCnv/ParticleLinksCnv_p1.h"
#include "ParticleEventTPCnv/SelectedParticlesCnv_p1.h"
#include "ParticleEventTPCnv/TrackParticleAssocsCnv_p1.h"

DECLARE_TPCNV_FACTORY(CompositeParticleCnv_p1,
                      CompositeParticle,
                      CompositeParticle_p1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(CompositeParticleContainerCnv_p1,
                      CompositeParticleContainer,
                      CompositeParticleContainer_p1,
                      Athena::TPCnvVers::Current)

/*
DECLARE_TPCNV_FACTORY(NeutrinoCnv_p1,
                      Neutrino,
                      Neutrino_p1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(NeutrinoCnv_p2,
                      Neutrino,
                      Neutrino_p2,
                      Athena::TPCnvVers::Current)
*/

DECLARE_TPCNV_FACTORY(NeutrinoContainerCnv_p1,
                      NeutrinoContainer,
                      NeutrinoContainer_p1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(NeutrinoContainerCnv_p2,
                      NeutrinoContainer,
                      NeutrinoContainer_p2,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(ParticleBaseCnv_p1,
                      ParticleBase,
                      ParticleBase_p1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(ParticleBaseCnv_p2,
                      ParticleBase,
                      ParticleBase_p2,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(ParticleBaseLinksCnv_p1,
                      ParticleLinks<ParticleBaseContainer>,
                      ParticleLinks_p1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(TrackParticleAssocsCnv_p1,
		      TrackParticleAssocs,
		      TrackParticleAssocs_p1,
		      Athena::TPCnvVers::Current)
