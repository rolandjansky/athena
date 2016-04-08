/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2007


#ifndef TRUTHTRAJECTORY_P1_H
#define TRUTHTRAJECTORY_P1_H

#include <vector>
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p1.h"
#include "CLIDSvc/CLASS_DEF.h"


// Use T/P separated HepMcParticleLink

namespace Trk {
  class TruthTrajectory_p1 : public std::vector<HepMcParticleLink_p1> {};
}

CLASS_DEF( Trk::TruthTrajectory_p1 , 185011228 , 1 )

#endif/*TRUTHTRAJECTORY_P1_H*/
