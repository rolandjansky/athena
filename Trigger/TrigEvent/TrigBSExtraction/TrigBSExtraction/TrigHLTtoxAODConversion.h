/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBSEXTRACTION_TRIGHLTTOXAODCONVERSION_H
#define TRIGBSEXTRACTION_TRIGHLTTOXAODCONVERSION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigNavigation/Navigation.h"
#include "TrigBSExtraction/TrigBStoxAODTool.h"



namespace HLT {  
  class HLTResult;
}


/**
 * @brief Top algorithms which converts AOD containers referenced by the HLT to xAOD (when possible) and adjusts navigation accordingly
 */

class TrigHLTtoxAODConversion : public AthAlgorithm {
public:
  TrigHLTtoxAODConversion(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigHLTtoxAODConversion();   
  StatusCode initialize(); 
  StatusCode finalize();
  StatusCode execute();

private:
  ToolHandle<HLT::Navigation> m_navigation; //!< handle to Navigation tools
  ToolHandle<ITrigBStoxAODTool> m_xAODTool;
  std::string m_ResultKey;
};


#endif // TRIGBSEXTRACTION_TRIGHLTTOXAODCONVERSION_H
