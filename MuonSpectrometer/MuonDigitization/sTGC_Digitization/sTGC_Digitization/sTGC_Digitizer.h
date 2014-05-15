/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDIGITIZATION_STGC_DIGITIZER_H
#define MUONDIGITIZATION_STGC_DIGITIZER_H 

/// Gaudi External 
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h" 
/*******************************************************************************/
class IMuonDigitizationTool;
/*******************************************************************************/ 
class sTGC_Digitizer : public AthAlgorithm {

 public:

  sTGC_Digitizer(const std::string& name, ISvcLocator* pSvcLocator);
  ~sTGC_Digitizer();
    
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  ToolHandle<IMuonDigitizationTool> m_digTool;   
};
/*******************************************************************************/
#endif // MUONDIGITIZATION_STGC_DIGITIZER_H
/*******************************************************************************/

