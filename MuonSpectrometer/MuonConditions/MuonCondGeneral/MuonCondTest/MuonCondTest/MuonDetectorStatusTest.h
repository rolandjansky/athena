/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDTEST_MUONDETECTORSTATUSTEST_H
#define MUONCONDTEST_MUONDETECTORSTATUSTEST_H


#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "MuonCondSvc/MuonDetectorStatusDbSvc.h"

class MuonDetectorStatusTest : public AthAlgorithm {

 public:

  MuonDetectorStatusTest(const std::string& name, ISvcLocator* pSvcLocator);
  ~MuonDetectorStatusTest();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();

 private:
  MuonDetectorStatusDbSvc* m_muonDetectorStatusDbSvc;
  MsgStream* m_log;

 
  // the chrono service
  IChronoStatSvc* m_chronoSvc;

  std::string m_chrono1;
 

};



#endif     //MUONCONDTEST_MUONDETECTORSTATUSTEST_H
