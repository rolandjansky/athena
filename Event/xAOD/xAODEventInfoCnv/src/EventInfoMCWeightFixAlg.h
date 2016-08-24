/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTINFOMGT_EVENTINFOMCWEIGHTFIXALG_H
#define EVENTINFOMGT_EVENTINFOMCWEIGHTFIXALG_H 1

#include "AthenaBaseComps/AthAlgorithm.h"

//Author: Will Buttinger
//This algorithm is to fix the ATLASSIM-2989 issue with mc weights in EventInfo
//It should be used in AODFix for 20.7 xAOD



class EventInfoMCWeightFixAlg: public ::AthAlgorithm { 
 public: 
  EventInfoMCWeightFixAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~EventInfoMCWeightFixAlg(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private: 

}; 

#endif //> !EVENTINFOMGT_EVENTINFOMCWEIGHTFIXALG_H
