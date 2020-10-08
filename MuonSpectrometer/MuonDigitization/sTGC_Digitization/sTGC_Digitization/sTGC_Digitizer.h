/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDIGITIZATION_STGC_DIGITIZER_H
#define MUONDIGITIZATION_STGC_DIGITIZER_H 

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IPileUpTool;

class sTGC_Digitizer : public AthAlgorithm {

 public:
  sTGC_Digitizer(const std::string& name, ISvcLocator* pSvcLocator);
  ~sTGC_Digitizer()=default;
    
  StatusCode initialize();
  StatusCode execute();

 private:
  ToolHandle<IPileUpTool> m_digTool{this, "DigitizationTool", "sTgcDigitizationTool", "sTgcDigitizationTool name"};
};
/*******************************************************************************/
#endif // MUONDIGITIZATION_STGC_DIGITIZER_H
/*******************************************************************************/

