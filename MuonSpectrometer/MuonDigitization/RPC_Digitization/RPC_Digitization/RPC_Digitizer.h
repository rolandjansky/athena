/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDIGITIZATION_RPC_DIGITIZER_H
#define MUONDIGITIZATION_RPC_DIGITIZER_H 

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class IMuonDigitizationTool;

class RPC_Digitizer : public AthAlgorithm {

 public:

  RPC_Digitizer(const std::string& name, ISvcLocator* pSvcLocator);
  ~RPC_Digitizer();
    
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

   ToolHandle<IMuonDigitizationTool> m_digTool;
};

#endif // MUONDIGITIZATION_RPC_DIGITIZER_H


