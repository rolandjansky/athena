/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// declare the T/P converter factories
#include "AthenaKernel/TPCnvFactory.h"
#include "AthExDecayCnv_p1.h"
#include "AthExElephantinoCnv_p1.h"
#include "AthExFatObjectCnv_p1.h"
#include "AthExParticlesCnv_p1.h"
//#include "AthExThinning/AthExDecay_p1.h"
//#include "AthExThinning/AthExElephantino_p1.h"
//#include "AthExThinning/AthExFatObject_p1.h"
//#include "AthExThinning/AthExParticles_p1.h"

DECLARE_TPCNV_FACTORY(AthExDecayCnv_p1,
                      AthExDecay,
                      AthExDecay_p1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(AthExElephantinoCnv_p1,
                      AthExElephantino,
                      AthExElephantino_p1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(AthExFatObjectCnv_p1,
                      AthExFatObject,
                      AthExFatObject_p1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(AthExParticlesCnv_p1,
                      AthExParticles,
                      AthExParticles_p1,
                      Athena::TPCnvVers::Current)
