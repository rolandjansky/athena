/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DIGITIZATIONTESTS_PILEUPEVENTINFOTEST_H
#define DIGITIZATIONTESTS_PILEUPEVENTINFOTEST_H

#include "AthenaBaseComps/AthAlgorithm.h"

class PileUpEventInfoTest : public AthAlgorithm {

public:

  PileUpEventInfoTest(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  StringProperty m_pileupInfo;
};

#endif
