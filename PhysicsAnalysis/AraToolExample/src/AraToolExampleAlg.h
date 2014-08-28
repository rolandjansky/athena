/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "AraToolExample/AnExampleAraToolWrapper.h"

/////////////////////////////////////////////////////////////////////////////

class AraToolExampleAlg: public AthAlgorithm {
 public:
  AraToolExampleAlg (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
 protected:

  ToolHandle< AnExampleAraToolWrapper > m_tool;

};
 
