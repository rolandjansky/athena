/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAMCNVTEST_CSCDIGITTOCSCRDO_H
#define MUONBYTESTREAMCNVTEST_CSCDIGITTOCSCRDO_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class IMuonDigitizationTool;

// Author: Ketevi A. Assamagan
// BNL, October 27, 2003

// Digitization algorithm for the CSC:
// get the hit container from Storegate
// loop over the hits
// digitize each hit
// loop over the digit and build the digit container
// store the digits in StoreGate

class CscDigitToCscRDO : public AthAlgorithm {

 public:

  CscDigitToCscRDO(const std::string& name, ISvcLocator* pSvcLocator);
  ~CscDigitToCscRDO();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
   ToolHandle<IMuonDigitizationTool> m_digTool;
};

#endif
