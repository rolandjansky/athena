/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASTSIDIGITIZATION_SCT_FASTDIGITIZATION_H
#define FASTSIDIGITIZATION_SCT_FASTDIGITIZATION_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class IPileUpTool;

/** Top algorithm class for SCT digitization */
class SCT_FastDigitization : public AthAlgorithm {

 public:

  /** Constructor with parameters */
  SCT_FastDigitization(const std::string &name,ISvcLocator *pSvcLocator);

  /** Basic algorithm methods */
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;

 private:
  ToolHandle<IPileUpTool> m_digTool{this, "DigitizationTool", "SCT_FastDigitizationTool", "AthAlgTool which performs the SCT digitization"};
};

#endif // FASTSIDIGITIZATION_SCT_FASTDIGITIZATION_H
