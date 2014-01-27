/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTracker/CollimatorSet.h"
#include "ForwardTracker/Collimator.h"
#include "ForwardTracker/CollimatorData.h"
#include "ForwardTracker/ConfigData.h"
#include "ForwardTracker/STLHelpers.h"
#include "ForwardTracker/ForwardTrackerConstants.h"

namespace ForwardTracker {
  
  Collimator::Container_t CollimatorSet(const ConfigData& cData, const Side& side) {
    
    Collimator::Container_t collimators;
    
    const CollimatorData& collData(cData);
    
    for (int i =0; i<collData.coll_n; ++i) {
      
      Collimator::ConstPtr_t col(new Collimator(collData.coll_z[side][i]*ForwardTrackerConstants::aside[side],
						collData.coll_a[side][i], 
						side));
      
      collimators.push_back(col);
    }
    
    std::sort(collimators.begin(), collimators.end(), absZGreater<Collimator::ConstPtr_t>);
    
    return collimators;
  }
}
