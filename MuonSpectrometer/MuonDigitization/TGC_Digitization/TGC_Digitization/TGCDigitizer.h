/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDIGITIZATION_TGC_DIGITIZER_H
#define MUONDIGITIZATION_TGC_DIGITIZER_H 

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IMuonDigitizationTool;

class TGCDigitizer : public AthAlgorithm {
 public:
  TGCDigitizer(const std::string& name, ISvcLocator* pSvcLocator);
  ~TGCDigitizer();
    
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
   ToolHandle<IMuonDigitizationTool> m_digTool;

};

#endif // MUONDIGITIZATION_TGC_DIGITIZER_H
