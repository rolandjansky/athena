/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPerformanceHistUtils/Probe.h"

// constructor
Probe::Probe(const xAOD::IParticle& tagTrack, const xAOD::IParticle& probeTrack) : 
  m_isMatched(false), m_tagTrack(tagTrack), m_probeTrack(probeTrack), m_sf_weight(1)
{}

// denstructor
Probe::~Probe()
{
}
