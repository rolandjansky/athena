/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTCALOMONTOOL_H
#define HLTCALOMONTOOL_H


#include <string>


double DeltaPhi(double phi1, double phi2);

double DeltaR(double eta1, double phi1, double eta2, double phi2);

std::string get_str(float number);


#endif
