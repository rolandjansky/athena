/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDIGITIZATION_MM_DIGITIZER_H
#define MUONDIGITIZATION_MM_DIGITIZER_H

/// Gaudi External
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IMuonDigitizationTool;

/*******************************************************************************/
class MM_Digitizer : public AthAlgorithm {

 public:

  MM_Digitizer(const std::string& name, ISvcLocator* pSvcLocator);
  ~MM_Digitizer();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  ToolHandle<IMuonDigitizationTool> m_digTool;

};
/*******************************************************************************/
#endif // MUONDIGITIZATION_MM_DIGITIZER_H


