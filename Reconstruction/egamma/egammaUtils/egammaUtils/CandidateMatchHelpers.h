/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CANDIDATEMATCHHELPERS_H
#define CANDIDATEMATCHHELPERS_H

#include <cmath>
#include <iostream>

namespace CandidateMatchHelpers{
  double CorrectedEta(const double clusterEta,const double z_first,const bool isEndCap);
  double PhiROT(const double Et,const double Eta, const int charge, const double r_first ,const bool isEndCap);
}


#endif
