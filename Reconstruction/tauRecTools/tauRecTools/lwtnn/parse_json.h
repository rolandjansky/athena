/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARSE_JSON_HH_TAURECTOOLS
#define PARSE_JSON_HH_TAURECTOOLS

#include "lightweight_network_config.h"

namespace lwtDev {
  // build feed forward variant
  JSONConfig parse_json(std::istream& json);
  // build graph variant
  GraphConfig parse_json_graph(std::istream& json);
}


#endif
