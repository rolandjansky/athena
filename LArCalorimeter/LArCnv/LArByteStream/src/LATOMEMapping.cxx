/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArByteStream/LATOMEMapping.h"

using namespace OFFLINE_FRAGMENTS_NAMESPACE;

typedef std::map<int, HWIdentifier> latome_map;

void add_to_map(latome_map* map, int value, int key) {
  HWIdentifier identifier(value);
  (*map)[key] = identifier;
}

void LATOMEMapping::fill(latome_map *toFill, std::string inputfile) {
  std::ifstream ifs(inputfile);
  if (ifs.fail()) std::cout << "Fail to read" << inputfile << std::endl;
  int value, key;
  while (ifs >> value >> key) {
    if (value != -999) add_to_map(toFill, value, key);
  }
}
