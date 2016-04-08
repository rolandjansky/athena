/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTEST_READEGAMMA_H
#define ANALYSISTEST_READEGAMMA_H

/*
  Read egamma

  @author Tadashi Maeno
*/

#include "GaudiKernel/NTuple.h"

#include "AnalysisTest/ReadConstituent.h"
#include "egammaEvent/egammaContainer.h"

class Readegamma : public ReadConstituent<egammaContainer>
{
public:
  Readegamma (const std::string& name, ISvcLocator* pSvcLocator)
    : ReadConstituent<egammaContainer> (name, pSvcLocator) {}
  ~Readegamma () {}

private:
  StatusCode userInit();
  StatusCode userExec(const egamma *val);

  NTuple::Array<double> m_e;
};

#endif







