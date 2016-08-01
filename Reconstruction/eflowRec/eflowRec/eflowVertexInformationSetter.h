/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWVERTEXINFORMATIONSETTER_H
#define EFLOWVERTEXINFORMATIONSETTER_H

/*

Class to add information to neutral EFO that is required to do a vertex correction

Author: Mark Hodgkinson
Created: 19 April 2013

*/

//Athena
#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>

class StoreGateSvc;

class eflowVertexInformationSetter : public AthAlgorithm {
  
 public:

  eflowVertexInformationSetter(const std::string& name, ISvcLocator* pSvcLocator);
  ~eflowVertexInformationSetter();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:

  std::string m_PFOName;

  StoreGateSvc* m_storeGate;

};
#endif
