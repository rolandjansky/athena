/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowBaseInterpolatedEEtaBinSystem.cxx
PACKAGE:  offline/Reconstruction/eflowRec

AUTHOR:   R Duxfield
CREATED:  17th May, 2006

********************************************************************/

#include <cmath>
#include "eflowRec/eflowBaseInterpolatedEEtaBinSystem.h"


////////////////////////////////
//   eflowBaseEEtaBinSystem   //
////////////////////////////////

const double eflowBaseInterpolatedEEtaBinSystem::m_errorReturnValue = -999.0;

eflowBaseInterpolatedEEtaBinSystem::~eflowBaseInterpolatedEEtaBinSystem() { }


void eflowBaseInterpolatedEEtaBinSystem::getInterpolation(const eflowParamEtaBin** bin1,
                                                          const eflowParamEtaBin** bin2, double& w1,
                                                          double e, double eta) const {
  int eBin = getEBinIndex(e);
  int etaBin = getEtaBinIndex(eta);

  /* Check for invalid bins */
  if (eBin < 0 || etaBin < 0) {
    *bin1 = 0;
    *bin2 = 0;
    w1 = -1.0;
    return;
  }

  if ((0 == eBin && e <= m_eBinBounds[eBin]) ||
      (eBin == getNumEBins() - 1 && e >= m_eBinBounds[getNumEBins() - 1])) {
    /* If e is below the lowest (above the highest) pinpoint, just return the lowest (highest) bin; no interpolation in this case. */
    *bin1 = m_bins[eBin][etaBin];
    *bin2 = m_bins[eBin][etaBin];
    w1 = 1.0;
  } else {
    /* The "normal" case: interpolate between two energies */
    int lowEBin = eBin;
    int highEBin = eBin;

    if (e >= m_eBinBounds[eBin]) {
      highEBin = eBin + 1;
    } else {
      lowEBin = eBin - 1;
    }

    *bin1 = m_bins[lowEBin][etaBin];
    *bin2 = m_bins[highEBin][etaBin];

    if (LIN == m_mode)
      w1 = (m_eBinBounds[highEBin] - e) / (m_eBinBounds[highEBin] - m_eBinBounds[lowEBin]);
    else
      w1 = log(m_eBinBounds[highEBin] / e) / log(m_eBinBounds[highEBin] / m_eBinBounds[lowEBin]);
  }
}


int eflowBaseInterpolatedEEtaBinSystem::getEBinIndex(double e) const {
  if (LOG == m_mode) {

    int nEBins = getNumEBins();
    int bin = 0;
    for (int i = nEBins - 1; i > 0; i--) {
      if (e > sqrt(m_eBinBounds[i - 1] * m_eBinBounds[i])) {
        bin = i;
        break;
      }
    }
    return bin;

  } else if (LIN == m_mode) {

    int nEBins = getNumEBins();
    int bin = 0;
    for (int i = nEBins - 1; i > 0; i--) {
      if (e > (m_eBinBounds[i - 1] * m_eBinBounds[i]) / 2.0) {
        bin = i;
        break;
      }
    }
    return bin;

  } else {
    return -1;
  }
}

int eflowBaseInterpolatedEEtaBinSystem::getEtaBinIndex(double eta) const {
  if (m_useAbsEta) eta = fabs(eta);

  /* If eta is outside bin range, return highest/lowest bin to avoid binning failures
   * (in practice we always use absEta, so it can only be too high) */
  if (eta > m_etaBinBounds.back()) {
    return m_etaBinBounds.size()-2; // Yes, it's minus *two* --> we need to return the index of the *low* edge of the bin
  }
  if (eta < m_etaBinBounds[0]){
    return 0;
  }

  return getBinIndex(eta, m_etaBinBounds);

}

int eflowBaseInterpolatedEEtaBinSystem::getBinIndex(double x, const std::vector<double>& binBounds) const {
  int nBins = binBounds.size() - 1;
  int bin = -1;
  for (int i = 0; i < nBins; i++) {
    if (x >= binBounds[i] && x < binBounds[i+1]) {
      bin = i;
      break;
    }
  }

  return bin;
}
