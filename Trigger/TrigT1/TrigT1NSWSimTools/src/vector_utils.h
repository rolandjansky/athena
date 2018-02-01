//  -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NSW_VECTOR_UTILS_H
#define NSW_VECTOR_UTILS_H

#include "tdr_typedefs.h"

#include <vector>

/**
   @brief vector to string utility functions for TDR study

   @author davide.gerbaudo@gmail.com
   @date 2013
 */
namespace nsw {
std::string vec2str(const nsw::vsize_t &vec);
std::string vec2str(const nsw::vpads_t &vec);

/// given a vector, return something like range(len(vec))
template <typename Container>
std::vector<size_t> indices(Container const & container) {
    std::vector<size_t> idx(container.size(), 0);
    size_t i=0;
    while(i<container.size()){
        idx[i]= i;
        i++;
    }
    return idx;
}

} // nsw

#endif
