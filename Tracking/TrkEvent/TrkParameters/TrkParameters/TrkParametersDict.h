/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKPARAMETERS_DICT_H
#define TRKPARAMETERS_DICT_H

// Needed to successfully generate the dictionary in standalone mode:
#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#   define EIGEN_DONT_VECTORIZE
#endif // __GCCXML__

#include "TrkParameters/TrackParameters.h"

namespace {
  struct GCCXML_DUMMY_INSTANTIATION_TRKPARAMETERS {
    Trk::CurvilinearParameters p1;
    std::vector<const Trk::TrackParameters*> p2;
  };
}

#endif // TRKPARAMETERS_DICT_H

