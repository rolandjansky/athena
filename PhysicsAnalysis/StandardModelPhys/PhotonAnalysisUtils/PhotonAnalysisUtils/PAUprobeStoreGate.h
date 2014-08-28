/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHOTONANALYSISUTILS_PAUPROBESTOREGATE_H
#define PHOTONANALYSISUTILS_PAUPROBESTOREGATE_H 1

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class StoreGateSvc;

class PAUprobeStoreGate : public AthAlgorithm{
 public:
  PAUprobeStoreGate(const std::string& name, ISvcLocator* pSvcLocator);
  ~PAUprobeStoreGate() {} ;
  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();
 private:
  StoreGateSvc* m_storegatesvc;
};

#endif
