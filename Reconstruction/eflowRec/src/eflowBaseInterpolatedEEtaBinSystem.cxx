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



eflowBaseInterpolatedEEtaBinSystem::eflowBaseInterpolatedEEtaBinSystem(const std::vector<double>& eBinBounds, const std::vector<double>& etaBinBounds, EnergyInterpolationMode mode, bool useAbsEta) :
  eflowBaseEEtaBinSystem::eflowBaseEEtaBinSystem(eBinBounds, etaBinBounds, useAbsEta),
  m_mode(mode)
{}



eflowBaseInterpolatedEEtaBinSystem::eflowBaseInterpolatedEEtaBinSystem(const std::vector<double>& eBinBounds, int nEtaBins, double etaMin, double etaMax, EnergyInterpolationMode mode, bool useAbsEta) :
  eflowBaseEEtaBinSystem::eflowBaseEEtaBinSystem(eBinBounds, nEtaBins, etaMin, etaMax, useAbsEta),
  m_mode(mode)
{}



bool eflowBaseInterpolatedEEtaBinSystem::getBin(eflowBaseInterpolatedEtaBin& bin, double e, double eta) const
{
  const eflowBaseEtaBin* bin1 = 0;
  const eflowBaseEtaBin* bin2 = 0;
  double w1 = -1.0;

  getInterpolation(&bin1, &bin2, w1, e, eta);

  if (bin1 && bin2 && w1 >= 0.0 && w1 <= 1.0) {
    getWeightedMeanBin(bin, bin1, bin2, w1);
    return true;
  }
  else {
    return false;
  }
}


void eflowBaseInterpolatedEEtaBinSystem::getInterpolation(const eflowBaseEtaBin** bin1, const eflowBaseEtaBin** bin2, double& w1, double e, double eta) const
{
  int eBin = getEBinIndex(e);
  int etaBin = getEtaBinIndex(eta);

  if (eBin >= 0 && etaBin >= 0) {
    
    if ( (0 == eBin && e <= m_eBinBounds[eBin]) || (getNumEBins() - 1 == eBin && e >= m_eBinBounds[getNumEBins()-1]) ) {

      *bin1 = m_bins[eBin][etaBin];
      *bin2 = m_bins[eBin][etaBin];
      w1 = 1.0;
    }
    else {

      int lowEBin = eBin;
      int highEBin = eBin;

      if (e >= m_eBinBounds[eBin])
	highEBin = eBin + 1;
      else
	lowEBin = eBin - 1;

      *bin1 = m_bins[lowEBin][etaBin];
      *bin2 = m_bins[highEBin][etaBin];

      if (LIN == m_mode)
	w1 = (m_eBinBounds[highEBin] - e) / (m_eBinBounds[highEBin] - m_eBinBounds[lowEBin]);
      else
	w1 = log(m_eBinBounds[highEBin] / e) / log(m_eBinBounds[highEBin] / m_eBinBounds[lowEBin]);
    }
  }
  else {
    
    *bin1 = 0;
    *bin2 = 0;
    w1 = -1.0;
  }  
}


int eflowBaseInterpolatedEEtaBinSystem::getEBinIndex(double e) const
{
  if (LOG == m_mode) {

    int nEBins = getNumEBins();
    int bin = 0;

    for (int i = nEBins - 1; i > 0; i--) {
      
      if (e > sqrt(m_eBinBounds[i-1] * m_eBinBounds[i])) {
	bin = i;
	break;
      }
    }

    return bin;

  }
  else if (LIN == m_mode) {

    int nEBins = getNumEBins();
    int bin = 0;

    for (int i = nEBins - 1; i > 0; i--) {
      
      if ( e > (m_eBinBounds[i-1] * m_eBinBounds[i]) / 2.0 ) {
	bin = i;
	break;
      }
    }

    return bin;

  }
  else {
    
    return -1;
  }
}
