/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTEST_READTAUJET_H
#define ANALYSISTEST_READTAUJET_H

/*
  Read TauJets

  @author Tadashi Maeno
*/

#include "AnalysisTest/ReadParticle.h"
#include "tauEvent/TauJetContainer.h"

class ReadTauJet : public ReadParticle<Analysis::TauJetContainer>
{
public:
  ReadTauJet (const std::string& name, ISvcLocator* pSvcLocator)
    : ReadParticle<Analysis::TauJetContainer> (name, pSvcLocator) {}
};

#endif







