/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTEST_READPHOTON_H
#define ANALYSISTEST_READPHOTON_H

/*
  Read Photon

  @author Tadashi Maeno
*/

#include "AnalysisTest/ReadParticle.h"
#include "egammaEvent/PhotonContainer.h"

class ReadPhoton : public ReadParticle<PhotonContainer>
{
public:
  ReadPhoton (const std::string& name, ISvcLocator* pSvcLocator)
    : ReadParticle<PhotonContainer> (name, pSvcLocator) {}
};

#endif







