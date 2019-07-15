/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "TrigConfHLTData/HLTUtils.h"

int main (int argc, char *argv []) {
  if (argc > 1) {
    std::cout << TrigConf::HLTUtils::string2hash(argv[1]) << std::endl;
  }
  return 0;
}
