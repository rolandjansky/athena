/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Note: please don't include any ROOT in here (i.e. xAOD::Jet or
// anything from the EDM), it's not a stable dependency (in particular
// the TString overloads change between releases).

#ifndef HBB_CONFIG_H
#define HBB_CONFIG_H

#include <set>
#include <map>
#include <string>

namespace lwt {
  struct GraphConfig;
}

namespace FlavorTagDiscriminants {

  struct HbbGraphConfig
  {
    std::set<std::string> subjet;
    std::set<std::string> fatjet;
    int n_subjets;
    std::map<std::string, std::map<std::string,double>> defaults;
  };
  HbbGraphConfig getHbbGraphConfig(const lwt::GraphConfig& cfg);

}

#endif

