/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAMCNVTEST_MUONRDOTOMUONDIGIT_H
#define MUONBYTESTREAMCNVTEST_MUONRDOTOMUONDIGIT_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class IMuonDigitizationTool;

class MuonRdoToMuonDigit : public AthAlgorithm {

 public:

  MuonRdoToMuonDigit(const std::string& name, ISvcLocator* pSvcLocator);
  ~MuonRdoToMuonDigit();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

   ToolHandle<IMuonDigitizationTool> m_digTool;
};

#endif

