/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNEVENT_ALIGNPARLIST_H
#define TRKALIGNEVENT_ALIGNPARLIST_H

#include "boost/multi_array.hpp"

namespace Trk {
  class AlignPar;
  typedef boost::multi_array<AlignPar*,2> AlignParList;
  typedef boost::array<boost::multi_array_types::index, 2> AlignParListDims;
}

#endif // TRKALIGNEVENT_ALIGNPARLIST_H

