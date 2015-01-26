/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TOPOMONITORING_L1TOPOTESTALG_H
#define L1TOPOMONITORING_L1TOPOTESTALG_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
class L1TopoTestAlg: public AthAlgorithm {
 public:
  L1TopoTestAlg(const std::string& name, ISvcLocator* pSvcLocator );
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
 private:
  int m_blah;
};

#endif // L1TOPOMONITORING_L1TOPOTESTALG_H
