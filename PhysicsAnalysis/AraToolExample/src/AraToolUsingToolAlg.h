/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AraToolUsingToolAlg_h
#define AraToolUsingToolAlg_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "AraToolExample/AnExampleAraToolWrapper.h"
#include "AraToolExample/AraToolUsingToolWrapper.h"

/////////////////////////////////////////////////////////////////////////////

class AraToolUsingToolAlg: public AthAlgorithm {
 public:
  AraToolUsingToolAlg (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
 protected:

  ToolHandle< AnExampleAraToolWrapper > m_tool;
  ToolHandle< AraToolUsingToolWrapper > m_toolusingtool;

};
 
#endif
