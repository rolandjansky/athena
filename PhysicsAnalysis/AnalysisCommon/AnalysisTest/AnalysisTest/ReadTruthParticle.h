/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTEST_READTRUTHPARTICLE_H
#define ANALYSISTEST_READTRUTHPARTICLE_H

/*
  Read TruthParticles

  @author Tadashi Maeno
*/

#include "AnalysisTest/ReadParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"

class ReadTruthParticle : public ReadParticle<TruthParticleContainer>
{
public:
  ReadTruthParticle (const std::string& name, ISvcLocator* pSvcLocator)
    : ReadParticle<TruthParticleContainer> (name, pSvcLocator) {}
};

#endif







