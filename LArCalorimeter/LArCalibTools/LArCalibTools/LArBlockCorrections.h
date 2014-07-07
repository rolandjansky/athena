/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Algorithm.h"

class LArBlockCorrections:public Algorithm {
 public:
  LArBlockCorrections(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute() {return StatusCode::SUCCESS;}
  StatusCode finalize() {return StatusCode::SUCCESS;}

};
