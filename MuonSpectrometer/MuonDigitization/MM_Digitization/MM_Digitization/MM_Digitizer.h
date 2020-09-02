/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDIGITIZATION_MM_DIGITIZER_H
#define MUONDIGITIZATION_MM_DIGITIZER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IPileUpTool;

class MM_Digitizer : public AthAlgorithm {

 public:
  MM_Digitizer(const std::string& name, ISvcLocator* pSvcLocator);
  ~MM_Digitizer()=default;

  StatusCode initialize();
  StatusCode execute();
 private:
  ToolHandle<IPileUpTool> m_digTool{this, "DigitizationTool", "MM_DigitizationTool", "MMDigitizationTool name"};

};
#endif // MUONDIGITIZATION_MM_DIGITIZER_H


