/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowEEtaBinBase.cxx
PACKAGE:  offline/Reconstruction/eflowRec

AUTHOR:   R Duxfield
CREATED:  17th May, 2006

********************************************************************/

#include <cmath>
#include "eflowRec/eflowEEtaBinBase.h"


////////////////////////////////
//   eflowBaseEEtaBinSystem   //
////////////////////////////////

const double eflowEEtaBinBase::m_errorReturnValue = -999.0;

eflowEEtaBinBase::~eflowEEtaBinBase() { }


int eflowEEtaBinBase::getEBinIndex(double e) const {
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

int eflowEEtaBinBase::getEtaBinIndex(double eta) const {
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

int eflowEEtaBinBase::getBinIndex(double x, const std::vector<double>& binBounds) {
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
