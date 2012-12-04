/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTEST_READMUON_H
#define ANALYSISTEST_READMUON_H

/*
  Read Muons

  @author Tadashi Maeno
*/

#include "AnalysisTest/ReadParticle.h"
#include "muonEvent/MuonContainer.h"

class ReadMuon : public ReadParticle<Analysis::MuonContainer>
{
public:
  ReadMuon (const std::string& name, ISvcLocator* pSvcLocator)
    : ReadParticle<Analysis::MuonContainer> (name, pSvcLocator) {}
};

#endif







