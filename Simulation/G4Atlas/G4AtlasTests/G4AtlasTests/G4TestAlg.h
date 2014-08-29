/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTESTS_G4TESTALG_H
#define G4ATLASTESTS_G4TESTALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class ISimTestTool;

class G4TestAlg : public AthAlgorithm {

public:

  G4TestAlg(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();

private:
  ToolHandleArray<ISimTestTool> m_testTools;

};

#endif //G4ATLASTESTS_G4TESTALG_H
