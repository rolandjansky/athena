/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAMCNVTEST_CSCDIGITTOCSCRDO_H
#define MUONBYTESTREAMCNVTEST_CSCDIGITTOCSCRDO_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"

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
  ~CscDigitToCscRDO()=default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual bool isClonable() const override final { return true; }

 private:
   ToolHandle<IMuonDigitizationTool> m_digTool{this};
};

#endif
