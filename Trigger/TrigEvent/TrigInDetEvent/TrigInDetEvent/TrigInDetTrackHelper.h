/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETTRACKHELPER_H
#define TRIGINDETTRACKHELPER_H
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
//
// Class for operations based on Track Fit Parameters:
//

class TrigInDetTrackHelper {
  
 public:
  TrigInDetTrackHelper(const TrigInDetTrackFitPar* par): m_param(par) {}
  ~TrigInDetTrackHelper(){};

  // give phiC and etaC of point of intercept of track with cylinder of centre (0,0)
  // radius rC and half-length zC

void extrapolate(double rC, double zC, double &phiC, double &etaC) const;

 private:
  
  const TrigInDetTrackFitPar* m_param;
};
#endif
