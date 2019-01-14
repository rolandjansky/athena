/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1NSWSimTools/vector_utils.h"

#include "TrigT1NSWSimTools/PadWithHits.h"
#include <sstream>
#include <iterator>

namespace NSWL1 {

std::string vec2str(const std::vector< size_t > &vec){
    std::ostringstream oss;
    oss<<"[";
    if(!vec.empty()) {
        std::copy(vec.begin(), vec.end()-1, std::ostream_iterator<int>(oss, ","));
        oss << vec.back(); // avoid trailing ','
    }
    oss<<"]";
    return oss.str();
}

} // nsw
