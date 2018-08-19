/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1NSWSimTools/vector_utils.h"

#include "TrigT1NSWSimTools/PadWithHits.h"
#include <sstream>
#include <iterator>

namespace NSWL1 {

std::string vec2str(const vsize_t &vec){
    std::ostringstream oss;
    oss<<"[";
    if(!vec.empty()) {
        std::copy(vec.begin(), vec.end()-1, std::ostream_iterator<int>(oss, ","));
        oss << vec.back(); // avoid trailing ','
    }
    oss<<"]";
    return oss.str();
}
std::string vec2str(const vpads_t &vec){
    std::ostringstream oss;
    for(vpads_t::const_iterator it=vec.begin(); it!=vec.end(); ++it)
        oss<<(*it)<<", ";
    return oss.str();
}

} // nsw
