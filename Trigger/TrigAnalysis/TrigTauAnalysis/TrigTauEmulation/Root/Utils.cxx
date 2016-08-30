/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauEmulation/Utils.h"
#include <cmath>

double Utils::DeltaR(const double eta1, const double phi1, const double eta2, const double phi2) {
  double dEta(eta1 - eta2);
  double dPhi(phi1 - phi2);
  if (dPhi > +M_PI) dPhi -= 2. * M_PI;
  if (dPhi < -M_PI) dPhi += 2. * M_PI;
  double dR(sqrt(dEta * dEta + dPhi * dPhi));
  return dR;
}

double Utils::DeltaR_Square(const double eta1, const double phi1, const double eta2, const double phi2) {
  double dEta(eta1 - eta2);
  double dPhi(phi1 - phi2);
  if (dPhi > +M_PI) dPhi -= 2. * M_PI;
  if (dPhi < -M_PI) dPhi += 2. * M_PI;
  double dR(dEta * dEta + dPhi * dPhi);
  return dR;
}

double Utils::L1_DeltaR_Square(const double eta1, const double phi1, const double eta2, const double phi2)
// DR Squared with L1 Precision
{
  double dEta(eta1 - eta2);
  double dPhi(phi1 - phi2);
  if (dPhi > +M_PI) dPhi -= 2. * M_PI;
  if (dPhi < -M_PI) dPhi += 2. * M_PI;
  double dR_Square = round(100 * (dEta * dEta + dPhi * dPhi));
  return dR_Square;
}

double Utils::DeltaPhi(const double phi1, const double phi2) {
  double dPhi(phi1 - phi2);
  if (dPhi > +M_PI) dPhi -= 2. * M_PI;
  if (dPhi < -M_PI) dPhi += 2. * M_PI;
  return dPhi;
}

double Utils::DeltaEta(const double eta1, const double eta2) {
  double dEta(eta1 - eta2);
  return fabs(dEta);
}

double Utils::L1_DeltaPhi(const double phi1, const double phi2) {
  double dPhi(phi1 - phi2);
  if (dPhi > +M_PI) dPhi -= 2. * M_PI;
  if (dPhi < -M_PI) dPhi += 2. * M_PI;
  dPhi = round(10 * dPhi);
  return dPhi;
}

double Utils::L1_DeltaEta(const double eta1, const double eta2) {
  double dEta(eta1 - eta2);
  dEta = round(10 * dEta);
  return fabs(dEta);
}

std::vector<std::string> Utils::splitNames(const std::string& files, const std::string& sep) {
  std::vector<std::string> fileList;
  for (size_t i = 0, n; i <= files.length(); i = n + 1) {
    n = files.find_first_of(sep, i);
    if (n == std::string::npos) {
      n = files.length();
    }
    std::string tmp = files.substr(i, n - i);
    std::string ttmp;
    for (unsigned int j = 0; j < tmp.size(); j++) {
      if (tmp[j] == ' ' || tmp[j] == '\n') {
        continue;
      }
      ttmp += tmp[j];
    }
    fileList.push_back(ttmp);
  }
  return fileList;
}
