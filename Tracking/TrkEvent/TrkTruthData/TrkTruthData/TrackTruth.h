/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef TRKPREPRAWDATATRUTH_TRACKTHRUTH_H
#define TRKPREPRAWDATATRUTH_TRACKTHRUTH_H

#include "GeneratorObjects/HepMcParticleLink.h"

/**
 *  MC particle associated with a reco track + the quality of match.
 */
class TrackTruth {

 public:
  /** default ctr is required by POOL converters */
  TrackTruth() : m_particleLink(),m_probability(0) {}
  
  TrackTruth(const HepMcParticleLink& particleLink, float probability,
	     int=0 /*FIXME: unused "flag" arg so that old code still compiles */
	     )
    : m_particleLink(particleLink), m_probability(probability)
  {}
  
  const HepMcParticleLink& particleLink() const { return m_particleLink; }

  float probability() const { return m_probability; }

 private:
  HepMcParticleLink m_particleLink;
  float m_probability; // parameter indicating the quality of match
};

#endif // TRKPREPRAWDATATRUTH_TRACKTHRUTH_H
