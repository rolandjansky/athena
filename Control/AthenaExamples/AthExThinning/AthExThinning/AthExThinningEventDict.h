// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXTHINNING_ATHEXTHINNINGEVENTDICT_H
#define ATHEXTHINNING_ATHEXTHINNINGEVENTDICT_H 1

#include "AthExThinning/AthExIParticles.h"
#include "AthExThinning/AthExParticles.h"
#include "AthExThinning/AthExDecay.h"
#include "AthExThinning/AthExElephantino.h"

#include "AthExThinning/AthExFatObject.h"
#include "AthExThinning/AthExFatObjectSlimmer.h"

/// Persistency

#include "AthExThinning/AthExParticles_p1.h"
#include "AthExThinning/AthExDecay_p1.h"
#include "AthExThinning/AthExElephantino_p1.h"
#include "AthExThinning/AthExFatObject_p1.h"

namespace AthExThinningDict {

  struct temp {
    std::vector<AthExParticle*> m_athExParticles;
  };

}

#endif // ATHEXTHINNING_ATHEXTHINNINGEVENTDICT_H
