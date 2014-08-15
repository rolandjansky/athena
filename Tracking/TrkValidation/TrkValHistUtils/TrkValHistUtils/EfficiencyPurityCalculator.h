/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVALHISTUTILS_EFFICIENCYPURITYCALCULATOR_H
#define TRKVALHISTUTILS_EFFICIENCYPURITYCALCULATOR_H

#include "TH1.h"
#include "TH2.h"

class EfficiencyPurityCalculator {
 public:
   enum axis { kX, kY, kZ };
   
  void inverseSum( TH1* in, TH1* out, int axis );
  void calculateIntegrated( TH1* all, TH1* matched, TH1* puri, int axis, int dim = 1);
  void calculateEfficiency( TH1* reference, TH1* matched, TH1* effPur );
  
};

#endif
