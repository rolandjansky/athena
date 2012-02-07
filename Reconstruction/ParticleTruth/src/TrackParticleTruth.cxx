/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleTruth/TrackParticleTruth.h"
#include <iostream>

TrackParticleTruth::TrackParticleTruth():
  m_particleLink(),m_probability(0)
{}

TrackParticleTruth::TrackParticleTruth(const HepMcParticleLink& particleLink, float probability): 
  m_particleLink(particleLink), m_probability(probability)
{}

TrackParticleTruth::TrackParticleTruth (const TrackParticleTruth& other)
    :m_particleLink(other.m_particleLink),
     m_probability(other.m_probability)
{}

TrackParticleTruth::~TrackParticleTruth()
{}

