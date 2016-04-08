/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCTExtensionAlg_H
#define SCTExtensionAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"

class SCTExtensionAlg:public AthAlgorithm {
    public:
    SCTExtensionAlg (const std::string& name, ISvcLocator* pSvcLocator);
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
};

#endif
