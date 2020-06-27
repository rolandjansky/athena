/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAMCNVTEST_MUONRDOTOMUONDIGIT_H
#define MUONBYTESTREAMCNVTEST_MUONRDOTOMUONDIGIT_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"

class MuonRdoToMuonDigit : public AthAlgorithm {

 public:

  MuonRdoToMuonDigit(const std::string& name, ISvcLocator* pSvcLocator);
  ~MuonRdoToMuonDigit()=default;

  StatusCode initialize();
  StatusCode execute();

 private:

   ToolHandle<IMuonDigitizationTool> m_digTool;
};

#endif

