/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASTSIDIGITIZATION_SISMEAREDDIGITIZATION_H
#define FASTSIDIGITIZATION_SISMEAREDDIGITIZATION_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class IPileUpTool;

/** Top algorithm class for Pixel digitization */
class SiSmearedDigitization : public AthAlgorithm {

 public:

  /** Constructor with parameters */
  SiSmearedDigitization(const std::string &name,ISvcLocator *pSvcLocator);

  /** Basic algorithm methods */
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;

 private:
  ToolHandle<IPileUpTool> m_smearTool{this, "DigitizationTool", "SiSmearedDigitizationTool", "AthAlgTool which performs the Pixel or SCT smearing"};
};

#endif // FASTSIDIGITIZATION_SISMEAREDDIGITIZATION_H
