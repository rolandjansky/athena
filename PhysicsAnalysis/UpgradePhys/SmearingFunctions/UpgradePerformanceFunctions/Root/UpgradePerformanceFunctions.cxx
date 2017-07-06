/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UPGRADEPERFORMANCEFUNCTIONS_CXX
#define UPGRADEPERFORMANCEFUNCTIONS_CXX

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"

UpgradePerformanceFunctions::UpgradePerformanceFunctions() {
  // This is the default constructor, so we use default settings
  m_layout = gold;
  m_avgMu = 200.;

  // Initialize the missing ET performance histograms
  // For now, this is done inside the MissingETPerformanceFunctions

  // Initialize the tracking performance histograms
  // For now, this is done inside the TrackingPerformanceFunctions

}

UpgradePerformanceFunctions::UpgradePerformanceFunctions(UpgradeLayout layout, double avgMu) {
  m_layout = layout;
  m_avgMu = avgMu;
  if (avgMu != 80. && avgMu != 140. && avgMu != 200.) {
    std::cout << "ERROR: mu value " << avgMu 
	      << " not allowed. Allowed values are 80, 140, 200.  Falling back to mu=" 
	      << m_avgMu << std::endl;
  } else {
    m_avgMu = avgMu;
  }
}

void UpgradePerformanceFunctions::setLayout(UpgradeLayout layout) {
  m_layout = layout;
}

void UpgradePerformanceFunctions::setAvgMu(double avgMu) {
  if (avgMu != 80. && avgMu != 140. && avgMu != 200.) {
    std::cout << "ERROR: mu value " << avgMu 
	      << " not allowed. Allowed values are 80, 140, 200.  Falling back to mu=" 
	      << m_avgMu << std::endl;
  } else {
    m_avgMu = avgMu;
  }

}

UpgradePerformanceFunctions::UpgradeLayout UpgradePerformanceFunctions::getLayout() {
  return m_layout;
}

double UpgradePerformanceFunctions::getAvgMu() {
  return m_avgMu;
}

#endif
