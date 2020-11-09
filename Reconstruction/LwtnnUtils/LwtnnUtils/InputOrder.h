// this is -*- C++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Any modifications to this file may be copied to lwtnn[1] without
// attribution.
//
// [1]: https::www.github.com/lwtnn/lwtnn

#ifndef LWTNN_UTILS_INPUT_ORDER_H
#define LWTNN_UTILS_INPUT_ORDER_H

#include <vector>
#include <string>

namespace lwt::atlas {

  // the user should specify what inputs they are going to feed to
  // the network. This is different from the ordering that the
  // network uses internally: some variables that are passed in
  // might be dropped or reorganized.
  typedef std::vector<
    std::pair<std::string, std::vector<std::string>>
    > order_t;

  struct InputOrder
  {
    order_t scalar;
    order_t sequence;
  };

}


#endif
