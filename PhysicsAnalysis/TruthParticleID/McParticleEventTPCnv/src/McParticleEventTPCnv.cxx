/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

// HepMc includes
#include "GeneratorObjects/McEventCollection.h"
#include "McParticleEvent/TruthEtIsolations.h"

#include "McParticleEventTPCnv/TruthEtIsolationsCnv_p1.h"
#include "McParticleEventTPCnv/TruthEtIsolationsContainerCnv_p1.h"
#include "McParticleEventTPCnv/TruthParticleContainerCnv_p1.h"
#include "McParticleEventTPCnv/TruthParticleContainerCnv_p2.h"
#include "McParticleEventTPCnv/TruthParticleContainerCnv_p3.h"
#include "McParticleEventTPCnv/TruthParticleContainerCnv_p4.h"
#include "McParticleEventTPCnv/TruthParticleContainerCnv_p5.h"
#include "McParticleEventTPCnv/TruthParticleContainerCnv_p6.h"

#include "McParticleEventTPCnv/RootTruthParticleContainerCnv_p4.h"

DECLARE_TPCNV_FACTORY(TruthEtIsolationsCnv_p1,
                      TruthEtIsolations,
                      TruthEtIsolations_p1,
                      Athena::TPCnvVers::Current)

DECLARE_NAMED_TPCNV_FACTORY(TruthEtIsolationsContainerCnv_p1,
                            TruthEtIsolationsContainerCnv_p1,
                            TruthEtIsolationsContainer,
                            TruthEtIsolationsContainer_p1,
                            Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(TruthParticleContainerCnv_p1,
                      TruthParticleContainer,
                      TruthParticleContainer_p1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TruthParticleContainerCnv_p2,
                      TruthParticleContainer,
                      TruthParticleContainer_p2,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TruthParticleContainerCnv_p3,
                      TruthParticleContainer,
                      TruthParticleContainer_p3,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TruthParticleContainerCnv_p4,
                      TruthParticleContainer,
                      TruthParticleContainer_p4,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TruthParticleContainerCnv_p5,
                      TruthParticleContainer,
                      TruthParticleContainer_p5,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TruthParticleContainerCnv_p6,
                      TruthParticleContainer,
                      TruthParticleContainer_p6,
                      Athena::TPCnvVers::Current)

typedef T_TPCnv<TruthParticleContainer, TruthParticleContainer_p4> 
RootTruthParticleContainerCnv_p4;

// DECLARE_NAMED_TPCNV_FACTORY(RootTruthParticleContainerCnv_p4,
//                             RootTruthParticleContainerCnv_p4,
//                             TruthParticleContainer,
//                             TruthParticleContainer_p4,
//                             Athena::TPCnvVers::Old)

typedef T_TPCnv<TruthParticleContainer, TruthParticleContainer_p4> 
RootTruthParticleContainerCnv_p4;
DECLARE_ARATPCNV_FACTORY(RootTruthParticleContainerCnv_p4,
                         TruthParticleContainer,
                         TruthParticleContainer_p4,
                         Athena::TPCnvVers::Old)

typedef T_TPCnv<TruthParticleContainer, TruthParticleContainer_p5> 
RootTruthParticleContainerCnv_p5;
DECLARE_ARATPCNV_FACTORY(RootTruthParticleContainerCnv_p5,
                         TruthParticleContainer,
                         TruthParticleContainer_p5,
                         Athena::TPCnvVers::Old)

typedef T_TPCnv<TruthParticleContainer, TruthParticleContainer_p6> 
RootTruthParticleContainerCnv_p6;
DECLARE_ARATPCNV_FACTORY(RootTruthParticleContainerCnv_p6,
                         TruthParticleContainer,
                         TruthParticleContainer_p6,
                         Athena::TPCnvVers::Current)

