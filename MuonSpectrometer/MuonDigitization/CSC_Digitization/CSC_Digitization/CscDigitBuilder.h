/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSC_DIGITIZATION_CSCDIGITBUILDER_H
#define CSC_DIGITIZATION_CSCDIGITBUILDER_H

// Author: Ketevi A. Assamagan
// BNL, October 27, 2003

// Digitization algorithm for the CSC:
// get the hit container from Storegate
// loop over the hits
// digitize each hit
// loop over the digit and build the digit container
// store the digits in StoreGate

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class IPileUpTool;

class CscDigitBuilder : public AthAlgorithm {

public:

  /** Constructor with parameters */
  CscDigitBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  /** Destructor */
  virtual ~CscDigitBuilder() = default;

  /** Basic algorithm methods */
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;

private:
  ToolHandle<IPileUpTool> m_digTool{this, "DigitizationTool", "CscDigitizationTool", "CscDigitizationTool name"};
};

#endif // CSC_DIGITIZATION_CSCDIGITBUILDER_H
