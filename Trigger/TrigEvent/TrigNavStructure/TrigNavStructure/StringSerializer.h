/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVSTRUCTURE_STRINGSERIALIZER_H
#define TRIGNAVSTRUCTURE_STRINGSERIALIZER_H
#include <vector>
#include <string>
#include <stdint.h>
namespace HLT {
  namespace StringSerializer {
    void serialize (const std::string& str, std::vector<uint32_t>& storage);
    void serialize (const std::vector<std::string>& strings, std::vector<uint32_t>& storage );
    
    size_t deserialize (std::vector<uint32_t>::const_iterator first,
			std::vector<uint32_t>::const_iterator last,
			std::string& str);
    
    size_t deserialize (std::vector<uint32_t>::const_iterator first,
			std::vector<uint32_t>::const_iterator last,
			std::vector<std::string>& strings);

}
}
#endif
