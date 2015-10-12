/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIDIGITIZATION_SISMEAREDDIGITIZATION_H
#define SIDIGITIZATION_SISMEAREDDIGITIZATION_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class ISiSmearedDigitizationTool;

/** Top algorithm class for Pixel digitization */
class SiSmearedDigitization : public AthAlgorithm {

 public:

  /** Constructor with parameters */
  SiSmearedDigitization(const std::string &name,ISvcLocator *pSvcLocator);

  /** Basic algorithm methods */
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:
  ToolHandle<ISiSmearedDigitizationTool> m_smearTool;
};

#endif // SIDIGITIZATION_SISMEAREDDIGITIZATION_H
