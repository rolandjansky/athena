/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASTSIDIGITIZATION_SCT_FASTDIGITIZATION_H
#define FASTSIDIGITIZATION_SCT_FASTDIGITIZATION_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class ISCT_FastDigitizationTool;

/** Top algorithm class for SCT digitization */
class SCT_FastDigitization : public AthAlgorithm {

 public:

  /** Constructor with parameters */
  SCT_FastDigitization(const std::string &name,ISvcLocator *pSvcLocator);

  /** Basic algorithm methods */
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:
   ToolHandle<ISCT_FastDigitizationTool> m_digTool;
};

#endif // FASTSIDIGITIZATION_SCT_FASTDIGITIZATION_H
