/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDIGITIZATION_MDT_DIGITIZER_H
#define MUONDIGITIZATION_MDT_DIGITIZER_H 

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class IMuonDigitizationTool;

class MDT_Digitizer : public AthAlgorithm {

 public:

  MDT_Digitizer(const std::string& name, ISvcLocator* pSvcLocator);
  ~MDT_Digitizer();
    
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

   ToolHandle<IMuonDigitizationTool> m_digTool;
};

#endif // MUONDIGITIZATION_MDT_DIGITIZER_H

