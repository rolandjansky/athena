/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2007
// Olivier.Arnaez@cern.ch, 2015


#ifndef TRUTHTRAJECTORY_P2_H
#define TRUTHTRAJECTORY_P2_H

#include <vector>
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p2.h"
#include "CLIDSvc/CLASS_DEF.h"


// Use T/P separated HepMcParticleLink

namespace Trk {
  class TruthTrajectory_p2 : public std::vector<HepMcParticleLink_p2> {};
}

CLASS_DEF( Trk::TruthTrajectory_p2 , 185111228 , 1 )

#endif/*TRUTHTRAJECTORY_P2_H*/
