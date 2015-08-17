/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASTTRT_DIGITIZATION_TRTFASTDIGITIZATION_H
#define FASTTRT_DIGITIZATION_TRTFASTDIGITIZATION_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class ITRTFastDigitizationTool;

/** Top algorithm class for TRT digitization */
class TRTFastDigitization : public AthAlgorithm {

 public:

  /** Constructor with parameters */
  TRTFastDigitization(const std::string &name,ISvcLocator *pSvcLocator);

  /** Basic algorithm methods */
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:
   ToolHandle<ITRTFastDigitizationTool> m_digTool;
};

#endif // FASTTRT_DIGITIZATION_TRTFASTDIGITIZATION_H
