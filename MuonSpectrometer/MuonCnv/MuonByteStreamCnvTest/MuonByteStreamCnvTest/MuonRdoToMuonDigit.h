/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAMCNVTEST_MUONRDOTOMUONDIGIT_H
#define MUONBYTESTREAMCNVTEST_MUONRDOTOMUONDIGIT_H 

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"

class IMuonDigitizationTool;

class MsgStream;

class MuonRdoToMuonDigit : public Algorithm {

 public:

  MuonRdoToMuonDigit(const std::string& name, ISvcLocator* pSvcLocator);
  ~MuonRdoToMuonDigit();
    
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

   ToolHandle<IMuonDigitizationTool> m_digTool;

   MsgStream* m_log;
   bool m_debug;
   bool m_verbose;

};

#endif

