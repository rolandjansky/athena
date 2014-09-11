/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DIGITIZATIONTESTS_DIGITESTALG
#define DIGITIZATIONTESTS_DIGITESTALG

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
//#include "DigitizationTests/IDigiTestTool.h"

class IDigiTestTool;

class DigiTestAlg : public AthAlgorithm {

public:

  DigiTestAlg(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();

private:
  ToolHandleArray<IDigiTestTool> m_testTools;

};

#endif
