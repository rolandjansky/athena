/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASTTRT_DIGITIZATION_TRTFASTDIGITIZATION_H
#define FASTTRT_DIGITIZATION_TRTFASTDIGITIZATION_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class IPileUpTool;

/** Top algorithm class for TRT digitization */
class TRTFastDigitization : public AthAlgorithm {

 public:

  /** Constructor with parameters */
  TRTFastDigitization(const std::string &name,ISvcLocator *pSvcLocator);

  /** Basic algorithm methods */
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;

 private:
  ToolHandle<IPileUpTool> m_digTool{this, "DigitizationTool", "TRTFastDigitizationTool", "AthAlgTool which performs the TRT digitization"};
};

#endif // FASTTRT_DIGITIZATION_TRTFASTDIGITIZATION_H
