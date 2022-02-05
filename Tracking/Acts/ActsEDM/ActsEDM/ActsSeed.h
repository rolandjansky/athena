// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSEDM_SEED_H
#define ACTSEDM_SEED_H 1

#include "Acts/Seeding/Seed.hpp"
#include "ActsEDM/ActsSpacePoint.h" 

#include "AthContainers/DataVector.h"

namespace ActsTrk {
  typedef Acts::Seed< ActsTrk::SpacePoint > Seed;
  typedef DataVector< Acts::Seed< ActsTrk::SpacePoint > > SeedContainer;
}

// Set up a CLID for the type:
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( ActsTrk::Seed, 207128231, 1 )
CLASS_DEF( ActsTrk::SeedContainer, 1261318102, 1)

#endif
