/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko, 2008

#ifndef TOPEDM_WRITETEST_H
#define TOPEDM_WRITETEST_H

#include "AthenaBaseComps/AthAlgorithm.h"

class TopEDM_WriteTest : public AthAlgorithm {
  std::string m_jetCollectionName;
  std::string m_electronCollectionName;
  std::string m_muonCollectionName;
  std::string m_missingETName;
public:

  TopEDM_WriteTest(const std::string &name,ISvcLocator *pSvcLocator);
  
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

};

#endif/*TOPEDM_WRITETEST_H*/
