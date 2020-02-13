/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DIGITIZATION_TRTDIGITIZATION_H
#define TRT_DIGITIZATION_TRTDIGITIZATION_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "PileUpTools/IPileUpTool.h"

/** Top algorithm class for TRT digitization */
class TRTDigitization : public AthAlgorithm {

public:

  /** Constructor with parameters */
  TRTDigitization(const std::string &name,ISvcLocator *pSvcLocator);

  virtual ~TRTDigitization() = default;

  /** Basic algorithm methods */
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual bool isClonable() const override final { return true; }

private:
  ToolHandle<IPileUpTool> m_digTool{this,"DigitizationTool", "TRTDigitizationTool", "AthAlgTool which performs the TRT digitization"};
};

#endif // TRT_DIGITIZATION_TRTDIGITIZATION_H
