/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTEST_READELECTRON_H
#define ANALYSISTEST_READELECTRON_H

/*
  Read Electron

  @author Tadashi Maeno
*/

#include "AnalysisTest/ReadParticle.h"
#include "egammaEvent/ElectronContainer.h"

class ReadElectron : public ReadParticle<ElectronContainer>
{
public:
  ReadElectron (const std::string& name, ISvcLocator* pSvcLocator)
    : ReadParticle<ElectronContainer> (name, pSvcLocator) {}
};

#endif







