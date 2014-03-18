/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2007


#ifndef TRUTHTRAJECTORY_P0_H
#define TRUTHTRAJECTORY_P0_H

#include <vector>
#include "GeneratorObjects/HepMcParticleLink.h"
#include "CLIDSvc/CLASS_DEF.h"


// TruthTrajectory_p0 is a copy of the pre-T/P separated TruthTrajectory.
// Thus it uses HepMcParticleLink, not HepMcParticleLink_p1

namespace Trk {
  struct TruthTrajectory_p0 : public std::vector<HepMcParticleLink> {};
}

CLASS_DEF( Trk::TruthTrajectory_p0 , 91942553 , 1 )

#endif/*TRUTHTRAJECTORY_P0_H*/
