/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CANDIDATEMATCHHELPERS_H
#define CANDIDATEMATCHHELPERS_H

#include <cmath>
#include <iostream>

namespace CandidateMatchHelpers{
  double CorrectedEta(double clusterEta,double z_first,bool isEndCap);
  double PhiROT(double Et,double Eta, int charge, double r_first ,bool isEndCap);
}


#endif
