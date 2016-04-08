/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTEST_READVXCONTAINER_H
#define ANALYSISTEST_READVXCONTAINER_H

/*
  Read VxContainer

  @author Tadashi Maeno
*/

#include "GaudiKernel/NTuple.h"

#include "AnalysisTest/ReadConstituent.h"
#include "VxVertex/VxContainer.h"

class ReadVxContainer : public ReadConstituent<VxContainer>
{
public:
  ReadVxContainer (const std::string& name, ISvcLocator* pSvcLocator)
    : ReadConstituent<VxContainer> (name, pSvcLocator) {}
  ~ReadVxContainer () {}

private:
  StatusCode userInit();
  StatusCode userExec(const Trk::VxCandidate *val);

  NTuple::Array<double> m_chi2;
};

#endif







