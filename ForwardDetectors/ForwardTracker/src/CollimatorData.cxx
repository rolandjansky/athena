/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTracker/CollimatorData.h"
#include "ForwardTracker/ConfigData.h"
#include "ForwardTracker/ForwardTrackerConstants.h"

#include <sstream>

namespace ForwardTracker {

  CollimatorData::CollimatorData(const ConfigData& cData): 
    coll_z(std::vector<std::vector<double> >(coll_n, std::vector<double>(coll_n))),
    coll_a(std::vector<std::vector<double> >(coll_n, std::vector<double>(coll_n))) {
    
    coll_z[beam1][0] = cData.positionC1;
    coll_z[beam1][1] = cData.positionC2;
    coll_z[beam2][0] = cData.positionC1;
    coll_z[beam2][1] = cData.positionC2;
    
    coll_a[beam1][0] = cData.apertureC1;
    coll_a[beam1][1] = cData.apertureC2;
    coll_a[beam2][0] = cData.apertureC1;
    coll_a[beam2][1] = cData.apertureC2;
  }
}
