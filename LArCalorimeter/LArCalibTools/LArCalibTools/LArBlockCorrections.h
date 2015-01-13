/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgorithm.h"

class LArBlockCorrections:public AthAlgorithm {
 public:
  LArBlockCorrections(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute() {return StatusCode::SUCCESS;}
  StatusCode finalize() {return StatusCode::SUCCESS;}

};
