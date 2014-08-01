/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowBaseEEtaBinSystem.cxx
PACKAGE:  offline/Reconstruction/eflowRec

AUTHOR:   R Duxfield
CREATED:  8th May, 2006

********************************************************************/

//#include <iostream>
#include <cmath>
#include "eflowRec/eflowBaseEEtaBinSystem.h"

const double eflowBaseEEtaBinSystem::m_errorReturnValue = -999.0;




/////////////////////////
//   eflowBaseEtaBin   //
/////////////////////////


eflowBaseEtaBin::eflowBaseEtaBin(){
  m_eMin = 0.0;
  m_eMax = 0.0;
  m_etaMin = 0.0;
  m_eMax = 0.0;
}

eflowBaseEtaBin::~eflowBaseEtaBin()
{}




////////////////////////////////
//   eflowBaseEEtaBinSystem   //
////////////////////////////////


eflowBaseEEtaBinSystem::eflowBaseEEtaBinSystem(const std::vector<double>& eBinBounds, const std::vector<double>& etaBinBounds, bool useAbsEta) :
  m_useAbsEta(useAbsEta),
  m_eBinBounds(eBinBounds),
  m_etaBinBounds(etaBinBounds)
{}


eflowBaseEEtaBinSystem::eflowBaseEEtaBinSystem(const std::vector<double>& eBinBounds, int nEtaBins, double etaMin, double etaMax, bool useAbsEta) :
  m_useAbsEta(useAbsEta),
  m_eBinBounds(eBinBounds),
  m_etaBinBounds(nEtaBins+1)
{
  for (int i = 0; i <= nEtaBins; i++)
    m_etaBinBounds[i] = etaMin + i * (etaMax - etaMin) / nEtaBins;
}

eflowBaseEEtaBinSystem::eflowBaseEEtaBinSystem() { m_useAbsEta = true; }


void eflowBaseEEtaBinSystem::initialise(const std::vector<double>& eBinBounds, const std::vector<double>& etaBinBounds, bool useAbsEta)
{
  m_useAbsEta = useAbsEta;
  m_eBinBounds = eBinBounds;
  m_etaBinBounds = etaBinBounds;

  createBins();
}


void eflowBaseEEtaBinSystem::initialise(const std::vector<double>& eBinBounds, int nEtaBins, double etaMin, double etaMax, bool useAbsEta)
{
  m_useAbsEta = useAbsEta;
  m_eBinBounds = eBinBounds;
  m_etaBinBounds.resize(nEtaBins + 1);

  for (int i = 0; i <= nEtaBins; i++)
    m_etaBinBounds[i] = etaMin + i * (etaMax - etaMin) / nEtaBins;

  createBins();
}


void eflowBaseEEtaBinSystem::createBins()
{
  int nEBins = getNumEBins();
  int nEtaBins = getNumEtaBins()-1;

  if (nEBins > 0 && nEtaBins > 0) {

    m_bins.resize(nEBins);

    for (int i = 0; i < nEBins; i++) {
    
      m_bins[i].resize(nEtaBins);
      
      for (int j = 0; j < nEtaBins; j++)
	m_bins[i][j] = makeNewEtaBin(i, j);
    }  
  }
}


eflowBaseEEtaBinSystem::~eflowBaseEEtaBinSystem()
{
  int nEBins = m_bins.size();

  for (int i = 0; i < nEBins; i++) {

    int nEtaBins = m_bins[i].size();
  
    for (int j = 0; j < nEtaBins; j++)
      delete m_bins[i][j];
  }
}


int eflowBaseEEtaBinSystem::getBinIndex(double x, const std::vector<double>& binBounds) const
{
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


int eflowBaseEEtaBinSystem::getEBinIndex(double e) const
{
  return getBinIndex(e, m_eBinBounds);
}


int eflowBaseEEtaBinSystem::getEtaBinIndex(double eta) const
{
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


double eflowBaseEEtaBinSystem::getEBinEnergyMin(int eBinIndex) const
{
  return (eBinIndex >= 0 && eBinIndex < (getNumEBins()-1)) ? m_eBinBounds[eBinIndex] : m_errorReturnValue;
}


double eflowBaseEEtaBinSystem::getEBinEnergyMax(int eBinIndex) const
{
  return (eBinIndex >= 0 && eBinIndex < (getNumEBins()-1)) ? m_eBinBounds[eBinIndex+1] : m_errorReturnValue;
}


double eflowBaseEEtaBinSystem::getEtaBinEtaMin(int etaBinIndex) const
{
  return (etaBinIndex >= 0 && etaBinIndex < (getNumEtaBins()-1)) ? m_etaBinBounds[etaBinIndex] : m_errorReturnValue;
}


double eflowBaseEEtaBinSystem::getEtaBinEtaMax(int etaBinIndex) const
{
  return (etaBinIndex >= 0 && etaBinIndex < (getNumEtaBins()-1)) ? m_etaBinBounds[etaBinIndex+1] : m_errorReturnValue;
}
