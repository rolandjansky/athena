/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowEEtaBinnedParameters.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson, R Duxfield (based on R.Duxfields Root package)
CREATED:  18th Aug, 2005

********************************************************************/

//Athena Headers
#include "eflowRec/eflowEEtaBinnedParameters.h"
#include "eflowRec/eflowFirstIntParameters.h"
#include "eflowRec/eflowRingSubtractionManager.h"

#include "GaudiKernel/SystemOfUnits.h"

//C++ Headers
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;
///////////////////////////////////
//   eflowEEtaBinnedParameters   //
///////////////////////////////////

eflowEEtaBinnedParameters::~eflowEEtaBinnedParameters() {
  int nEBins = m_bins.size();
  for (int i = 0; i < nEBins; i++) {
    int nEtaBins = m_bins[i].size();
    for (int j = 0; j < nEtaBins; j++) {
      delete m_bins[i][j];
    }
  }
}

void eflowEEtaBinnedParameters::initialise(const std::vector<double>& eBinBounds, const std::vector<double>& etaBinBounds, bool useAbsEta) {
  m_useAbsEta = useAbsEta;
  m_eBinBounds = eBinBounds;
  m_etaBinBounds = etaBinBounds;

  /* Create all the bins */
  int nEBins = getNumEBins();
  int nEtaBins = getNumEtaBins()-1;
  eflowParameters* dummy = 0;
  m_bins.assign(nEBins, std::vector<eflowParameters*>(nEtaBins, dummy));
  for (int iEBin = 0; iEBin < nEBins; iEBin++) {
    for (int iEtaBin = 0; iEtaBin < nEtaBins; iEtaBin++) {
      m_bins[iEBin][iEtaBin] = new eflowParameters();
    }
  }
}

 double eflowEEtaBinnedParameters::getInterpolation(const eflowParameters** bin1,
                                                          const eflowParameters** bin2,
                                                          double e, double eta) const {
  double weight;
  int eBin = getEBinIndex(e);
  int etaBin = getEtaBinIndex(eta);

  /* Check for invalid bins */
  if (eBin < 0 || etaBin < 0) {
    *bin1 = 0;
    *bin2 = 0;
    weight = -1.0;
    return weight;
  }

  if ((0 == eBin && e <= m_eBinBounds[eBin]) ||
      (eBin == getNumEBins() - 1 && e >= m_eBinBounds[getNumEBins() - 1])) {
    /* If e is below the lowest (above the highest) pinpoint, just return the lowest (highest) bin; no interpolation in this case. */
    *bin1 = m_bins[eBin][etaBin];
    *bin2 = m_bins[eBin][etaBin];
    weight = 1.0;
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

    if (LIN == m_mode) {
      weight = (m_eBinBounds[highEBin] - e) / (m_eBinBounds[highEBin] - m_eBinBounds[lowEBin]);
    } else {
      weight = log(m_eBinBounds[highEBin] / e) / log(m_eBinBounds[highEBin] / m_eBinBounds[lowEBin]);
    }
  }
  return weight;
}

eflowFirstIntENUM eflowEEtaBinnedParameters::adjustLFI(double e, double eta,
                                                   eflowFirstIntENUM j1st) const {
  int eBinIndex = getEBinIndex(e);
  //hard code this check - needs to be rechecked if ever remake e/p tables
  //if e.g track extrapolation slightly off can have inconsistent j1st and eta (e.g eta = 1.52, j1st = EMB2) and hence fudgeMean is zero
  if (0 <= eBinIndex && eBinIndex <= 5) {
    if (fabs(eta) < 1.5) {
      if (eflowFirstIntRegions::EME1 == j1st) j1st = eflowFirstIntRegions::EMB1;
      if (eflowFirstIntRegions::EME2 == j1st) j1st = eflowFirstIntRegions::EMB2;
      if (eflowFirstIntRegions::EME3 == j1st) j1st = eflowFirstIntRegions::EMB3;
      if (eflowFirstIntRegions::HEC == j1st) j1st = eflowFirstIntRegions::Tile;
    }
    if (fabs(eta) >= 1.6) {
      if (eflowFirstIntRegions::EMB1 == j1st) j1st = eflowFirstIntRegions::EME1;
      if (eflowFirstIntRegions::EMB2 == j1st) j1st = eflowFirstIntRegions::EME2;
      if (eflowFirstIntRegions::EMB3 == j1st) j1st = eflowFirstIntRegions::EME3;
      if (eflowFirstIntRegions::Tile == j1st) j1st = eflowFirstIntRegions::HEC;
    }

  }

  if (0 == eBinIndex || 1 == eBinIndex) {
    if (fabs(eta) >= 1.0 && fabs(eta) < 1.1) {
      if(eflowFirstIntRegions::Tile == j1st) j1st = eflowFirstIntRegions::EMB3;
    }
    else if (fabs(eta) >= 1.2 && fabs(eta) < 1.3) {
      if(eflowFirstIntRegions::Tile == j1st) j1st = eflowFirstIntRegions::EMB3;
    }
    else if (fabs(eta) >= 1.5 && fabs(eta) < 1.6) {
      if(eflowFirstIntRegions::EMB3 == j1st) j1st = eflowFirstIntRegions::EME3;
    }

  } else if (2 == eBinIndex) {
    if (fabs(eta) >= 1.5 && fabs(eta) < 1.6) {
      if(eflowFirstIntRegions::EMB2 == j1st) j1st = eflowFirstIntRegions::EME2;
    }

  } else if (3 == eBinIndex) {
    if (fabs(eta) >= 1.5 && fabs(eta) < 1.6) {
      if (eflowFirstIntRegions::EMB2 == j1st) j1st = eflowFirstIntRegions::EME2;
      if (eflowFirstIntRegions::EMB1 == j1st) j1st = eflowFirstIntRegions::EME1;
    }

  } else if (4 == eBinIndex || 5 == eBinIndex) {
    if (fabs(eta) >= 1.5 && fabs(eta) < 1.6) {
      if(eflowFirstIntRegions::EMB1 == j1st) j1st = eflowFirstIntRegions::EME1;
    }
  }

  return j1st;
}

bool eflowEEtaBinnedParameters::getOrdering(eflowRingSubtractionManager& subtMan, double e, double eta, eflowFirstIntENUM j1st) const {
  const eflowEEtaBinnedParameters * tmp = this;
  return subtMan.getOrdering(tmp, e, eta, j1st);
}
