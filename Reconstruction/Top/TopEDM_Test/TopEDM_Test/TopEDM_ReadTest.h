/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko, 2008

#ifndef TOPEDM_READTEST_H
#define TOPEDM_READTEST_H

#include "AthenaBaseComps/AthAlgorithm.h"

class TopEDM_ReadTest : public AthAlgorithm {
  std::string m_ttbarJJContainerName;
  std::string m_ttbarJJTopContainerName;
  std::string m_ttbarEJContainerName;
  std::string m_ttbarEJTopContainerName;
  std::string m_ttbarEJDiTopsContainerName;
  std::string m_ttbarEEContainerName;
  std::string m_ttbarEEJetsContainerName;
  std::string m_ttbarEEJetsMetContainerName;
  std::string m_ttbarEJMetContainerName;

public:

  TopEDM_ReadTest(const std::string &name,ISvcLocator *pSvcLocator);
  
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

};

#endif/*TOPEDM_READTEST_H*/
