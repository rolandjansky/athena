/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORINGUTILITIES
#define INDETPHYSVALMONITORINGUTILITIES
/**
 * @file InDetPhysValMonitoringUtilities.h
 * @author shaun roe
 * Header for generic utilities in the InDetPhysValMonitoring package
 **/

 #include <cmath>
 #include <vector>
namespace IDPVM {
  ///Convert to GeV from the default MeV
  constexpr float
  operator "" _GeV (long double energy) {
    return energy * 0.001;
  }

  constexpr float
  operator "" _GeV (unsigned long long energy) {
    return energy * 0.001;
  }

  ///general utility function to check value is in range
  template <class T>
  inline bool
  inRange(const T& value, const T& minVal, const T& maxVal) {
    return not ((value < minVal)or(value > maxVal));
  }

  template<class T>
  inline bool
  inRange(const T& value, const T& absoluteMax) {
    return not (std::abs(value) > absoluteMax);
  }

  ///general utility function to return bin index given a value and the upper endpoints of each bin
  template <class T>
  unsigned int
  binIndex(const T& val, const std::vector<T>& partitions) {// signature should allow other containers
    unsigned int i(0);
    bool nf = true;

    while (nf and i != partitions.size()) {
      nf = (val > partitions[i++]);
    }
    return nf ? i : i - 1;
  }

  ///Safely get eta
  template <class T>
  inline float
  safelyGetEta(const T& pTrk, const float safePtThreshold = 0.1) {
    return (pTrk.pt() > safePtThreshold) ? (pTrk.eta()) : std::nan("");
  }

  template <class T>
  inline float
  safelyGetEta(const T* pTrk, const float safePtThreshold = 0.1) {
    return (pTrk->pt() > safePtThreshold) ? (pTrk->eta()) : std::nan("");
  }
}

 #endif