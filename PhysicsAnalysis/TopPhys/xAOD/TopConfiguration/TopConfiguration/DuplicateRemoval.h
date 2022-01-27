/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef _UTILS_DUPLICATE_REMOVAL_H_
#define _UTILS_DUPLICATE_REMOVAL_H_

#include <vector>
#include <algorithm>

namespace top {
  // remove duplicate elements in input vector
  // performs sorting of the elements and remove duplicates via std::unique
  template <typename T>
  void remove_duplicates(std::vector<T>& input) {
    std::sort(input.begin(), input.end());
    input.erase(std::unique(input.begin(), input.end()), input.end());
  }
}

#endif
