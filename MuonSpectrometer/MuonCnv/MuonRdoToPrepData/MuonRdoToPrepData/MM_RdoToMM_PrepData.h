/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCRDOTOCSCPREPDATA_H
#define CSCRDOTOCSCPREPDATA_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class MM_RdoToMM_PrepData : public AthAlgorithm {

public:

  MM_RdoToMM_PrepData(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

};

#endif /// CSCRDOTOCSCPREPDATA_H

