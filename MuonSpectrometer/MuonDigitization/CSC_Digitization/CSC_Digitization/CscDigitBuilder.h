/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDIGITIZATION_CSCDIGITBUILDER_H
#define MUONDIGITIZATION_CSCDIGITBUILDER_H 

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

class CscDigitBuilder : public AthAlgorithm {

 public:

  CscDigitBuilder(const std::string& name, ISvcLocator* pSvcLocator);
  ~CscDigitBuilder();
    
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

   ToolHandle<IMuonDigitizationTool> m_digTool;
};

#endif // MUONDIGITIZATION_CSCDIGITBUILDER_H
