/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "./TrigConfMD5.h"

#include <sstream>

#include "CxxUtils/MD5.h"

namespace TrigConf {

  uint32_t truncatedHash(DataStructure& dataStructure) {
    std::stringstream ss;
    dataStructure.printRaw(ss);
    MD5 md5(ss.str());
    const std::string truncated_hash_hex = md5.hex_digest().substr(0, 8); // Of 32
    std::stringstream hex_to_int;
    uint32_t truncated_hash_int = 0;
    hex_to_int << std::hex << truncated_hash_hex;
    hex_to_int >> truncated_hash_int;
    return truncated_hash_int;
  }

}
