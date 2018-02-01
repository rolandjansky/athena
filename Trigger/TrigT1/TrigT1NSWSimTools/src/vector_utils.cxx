/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "vector_utils.h"

#include "PadWithHits.h"

#include <sstream>
#include <iterator>

namespace nsw {

std::string vec2str(const nsw::vsize_t &vec){
    std::ostringstream oss;
    oss<<"[";
    if(!vec.empty()) {
        std::copy(vec.begin(), vec.end()-1, std::ostream_iterator<int>(oss, ","));
        oss << vec.back(); // avoid trailing ','
    }
    oss<<"]";
    return oss.str();
}
std::string vec2str(const nsw::vpads_t &vec){
    std::ostringstream oss;
    for(nsw::vpads_t::const_iterator it=vec.begin(); it!=vec.end(); ++it)
        oss<<(*it)<<", ";
    return oss.str();
}

} // nsw
