/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDTEST_MDT_DCSSTATUSTEST_H
#define MUONCONDTEST_MDT_DCSSTATUSTEST_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "MuonCondSvc/MDT_DCSConditionsSvc.h"


class IMDT_DCSConditionsSvc;
class MDT_DCSStatusTest : public AthAlgorithm {

 public:

  MDT_DCSStatusTest(const std::string& name, ISvcLocator* pSvcLocator);
  ~MDT_DCSStatusTest();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();

 private:
  IMDT_DCSConditionsSvc* m_mdt_DCSConditionsSvc;
  MsgStream* m_log;

 
  // the chrono service
  IChronoStatSvc* m_chronoSvc;

  std::string m_chrono1;
 

};



#endif     //MUONCONDTEST_MDT_DCSSTATUSTEST_H
