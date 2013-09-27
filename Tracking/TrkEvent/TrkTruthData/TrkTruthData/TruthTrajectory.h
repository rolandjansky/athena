/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef TRUTHTRAJECTORY_H
#define TRUTHTRAJECTORY_H

#include <vector>
#include "GeneratorObjects/HepMcParticleLink.h"
#include "CLIDSvc/CLASS_DEF.h"

/**
 * @class TruthTrajectory
 *
 * A TruthTrajectory is a chain of charged MC particles connected through the
 * mother-daughter relationship.  We limit the chain to its "observed"
 * part by requiring that the first and the last particles have
 * produced hits in the detector.
 *
 * Particles in TruthTrajectory are stored in reverse order: the last
 * particle, uniquely ideintifying the trajectory, is at [0], and the
 * most primary on trajectory is at rbegin()
 */

class TruthTrajectory : public std::vector<HepMcParticleLink> {};

template<class Ostream> Ostream& operator<<(Ostream& os, const TruthTrajectory& m) {
  os<<"TruthTrajectory(";
  for(unsigned i=0; i<m.size(); ) {
    os<<m[i];
    if(++i<m.size()) os<<",";
  }
  return os<<")";
}

CLASS_DEF(TruthTrajectory, 115990571, 1)

#endif/*TRUTHTRAJECTORY_H*/
