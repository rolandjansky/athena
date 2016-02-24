/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAMVALAYERDEPTH_H
#define EGAMMAMVALAYERDEPTH_H

// helper function to compute shower depth
// copied and pasted from:
// https://svnweb.cern.ch/trac/atlasoff/browser/Calorimeter/CaloClusterCorrection/trunk/src/CaloSwCalibHitsShowerDepth.cxx
// https://svnweb.cern.ch/trac/atlasoff/browser/Calorimeter/CaloClusterCorrection/trunk/python/CaloSwCalibHitsCalibration_v9leakdata.py#L2639


#include <array>

std::array<float, 4> get_MVAradius(float eta);

#endif
