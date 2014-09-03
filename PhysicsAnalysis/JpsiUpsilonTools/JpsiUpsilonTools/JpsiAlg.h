/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "JpsiUpsilonTools/JpsiFinder.h"

class StoreGateSvc;

/////////////////////////////////////////////////////////////////////////////
class JpsiAlg : public AthAlgorithm {

public:
  JpsiAlg (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  int eventCntr;
  int jpsiCntr;
 
  ToolHandle<Analysis::JpsiFinder> m_jpsiFinder;
  std::string                      m_jpsiContainerName; //!< Name of output container to store results

};
 
