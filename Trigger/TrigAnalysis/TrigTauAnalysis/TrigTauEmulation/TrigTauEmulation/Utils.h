/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UTILS_H
#define UTILS_H
//#include <map>


namespace Utils {

  double DeltaR(const double eta1, const double phi1, const double eta2, const double phi2);
  double DeltaR_Square(const double eta1, const double phi1, const double eta2, const double phi2);
  double L1_DeltaR_Square(const double eta1, const double phi1, const double eta2, const double phi2);
  double L1_DeltaPhi(const double phi1, const double phi2);
  double L1_DeltaEta(const double eta1, const double eta2);
  double DeltaPhi(const double phi1, const double phi2);
  double DeltaEta(const double eta1, const double eta2);

  //  std::map<std::string, int> getName(const std::string & name);
}

#endif
