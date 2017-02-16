/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPerformanceHistUtils/Probe.h"

// constructor
Probe::Probe(const xAOD::IParticle& tagTrack, const xAOD::IParticle& probeTrack) : 
    dRL1(-1),
    dRHLT(-1),
    m_isMatched(false), 
    m_tagTrack(tagTrack), 
    m_probeTrack(probeTrack), 
    m_sf_reco(1),
    m_sf_isol(1),
    m_sf_trig(1),
    m_tagPassTrigger(0),
    m_HasSomeTrigger(false),
    m_HasSomeTrigger_HLT(false),
    m_vx(0)
{}

Probe::Probe(const xAOD::IParticle& tagTrack, const xAOD::IParticle& probeTrack, int tagPassTrigger) : 
    dRL1(-1),
    dRHLT(-1),
    m_isMatched(false), 
    m_tagTrack(tagTrack), 
    m_probeTrack(probeTrack), 
    m_sf_reco(1),
    m_sf_isol(1),
    m_sf_trig(1),
    m_tagPassTrigger(tagPassTrigger),
    m_HasSomeTrigger(false),
    m_HasSomeTrigger_HLT(false),
    m_vx(0)
{}

// denstructor
Probe::~Probe()
{
}

void Probe::setVertex(const xAOD::Vertex* vertex){
    m_vx = vertex;
}
const xAOD::Vertex* Probe::getVertex(){
    return m_vx;
}
