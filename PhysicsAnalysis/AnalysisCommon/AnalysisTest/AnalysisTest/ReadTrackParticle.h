/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTEST_READTRACKPARTICLE_H
#define ANALYSISTEST_READTRACKPARTICLE_H

/*
  Read TrackParticle

  @author Tadashi Maeno
*/

#include "GaudiKernel/NTuple.h"

#include "AnalysisTest/ReadConstituent.h"
#include "Particle/TrackParticleContainer.h"

class ReadTrackParticle : public ReadConstituent<Rec::TrackParticleContainer>
{
public:
  ReadTrackParticle (const std::string& name, ISvcLocator* pSvcLocator)
    : ReadConstituent<Rec::TrackParticleContainer> (name, pSvcLocator) {}
  ~ReadTrackParticle () {}

private:
  StatusCode userInit();
  StatusCode userExec(const Rec::TrackParticle *val);

  NTuple::Array<double> m_charge;
};

#endif







