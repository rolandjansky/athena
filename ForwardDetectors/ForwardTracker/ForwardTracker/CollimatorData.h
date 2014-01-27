/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDTRACKER_COLLIMATORDATA_H
#define FORWARDTRACKER_COLLIMATORDATA_H

#include <vector>

namespace ForwardTracker {

  class ConfigData;
  
  struct CollimatorData {

    static const int coll_n = 2;
    
    std::vector< std::vector<double> > coll_z; // absolute values, collimator front faces
    std::vector< std::vector<double> > coll_a; // xy aperture
    
    CollimatorData(const ConfigData&);
  };
}

#endif
