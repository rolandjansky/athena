/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INTERPRET_SEEDS_H
#define INTERPRET_SEEDS_H 1
#include <string>
#include <stdint.h>
bool interpretSeeds(const std::string& buffer, 
		    std::string& stream, uint32_t& seed1, uint32_t& seed2,
		    short& luxury, uint32_t& offset);
inline
bool interpretSeeds(const std::string& buffer, 
		    std::string& stream, uint32_t& seed1, uint32_t& seed2,
		    uint32_t& offset) {
  short luxury(0);
  return interpretSeeds(buffer, stream, seed1, seed2, luxury, offset);
}

inline
bool interpretSeeds(const std::string& buffer, 
		    std::string& stream, uint32_t& seed1, uint32_t& seed2) {
  short luxury(0);
  uint32_t offset(0);
  return interpretSeeds(buffer, stream, seed1, seed2, luxury, offset);
}

bool interpretSeeds(const std::string& buffer, 
		    std::string& stream, std::vector<uint32_t>& seeds);
#endif
