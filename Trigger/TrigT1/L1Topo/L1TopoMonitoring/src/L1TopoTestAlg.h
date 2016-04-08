/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TOPOMONITORING_L1TOPOTESTALG_H
#define L1TOPOMONITORING_L1TOPOTESTALG_H 1

#include "AthenaBaseComps/AthAlgorithm.h"


namespace TrigConf {
  class IL1TopoConfigSvc;
}

class L1TopoTestAlg: public AthAlgorithm {
 public:
  L1TopoTestAlg(const std::string& name, ISvcLocator* pSvcLocator );
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
 private:
  ServiceHandle<TrigConf::IL1TopoConfigSvc> m_l1topoConfigSvc;
};

#endif // L1TOPOMONITORING_L1TOPOTESTALG_H
