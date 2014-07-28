/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DIGITIZATION_TRTDIGITIZATION_H
#define TRT_DIGITIZATION_TRTDIGITIZATION_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class TRTDigitizationTool;

/** Top algorithm class for TRT digitization */
class TRTDigitization : public AthAlgorithm {

 public:

  /** Constructor with parameters */
  TRTDigitization(const std::string &name,ISvcLocator *pSvcLocator);

  /** Basic algorithm methods */
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:
   ToolHandle<TRTDigitizationTool> m_digTool;
};

#endif // TRT_DIGITIZATION_TRTDIGITIZATION_H
