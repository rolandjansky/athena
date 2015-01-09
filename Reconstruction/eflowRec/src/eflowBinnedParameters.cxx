/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowBinnedParameters.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson, R Duxfield (based on R.Duxfields Root package)
CREATED:  18th Aug, 2005

********************************************************************/

//Athena Headers
#include "eflowRec/eflowBinnedParameters.h"
#include "eflowRec/eflowFirstIntParameters.h"
#include "eflowRec/eflowCellSubtractionManager.h"

#include "GaudiKernel/SystemOfUnits.h"

//C++ Headers
#include <cmath>
#include <fstream>
#include<iostream>

using namespace std;



//////////////////////////
//   eflowParamEtaBin   //
//////////////////////////


eflowParamEtaBin::eflowParamEtaBin() :
    m_parentSystem(0), m_ringThicknesses(eflowCalo::nRegions) {
  int nSubtRegions = eflowFirstIntRegions::nRegions;
  m_firstIntBins.resize(nSubtRegions);
  for (int i = 0; i < nSubtRegions; i++) {
    m_firstIntBins[i] = new eflowFirstIntParameters;
  }
}

eflowParamEtaBin::eflowParamEtaBin(const eflowBinnedParameters* parentSystem) :
  m_parentSystem(parentSystem),
  m_ringThicknesses(eflowCalo::nRegions) {
  int nSubtRegions = eflowFirstIntRegions::nRegions;
  m_firstIntBins.resize(nSubtRegions);
  for (int i = 0; i < nSubtRegions; i++) {
    m_firstIntBins[i] = new eflowFirstIntParameters;
  }
}

eflowParamEtaBin::~eflowParamEtaBin() {
  int n = m_firstIntBins.size();
  for (int i = 0; i < n; i++) {
    delete m_firstIntBins[i];
  }
}

///////////////////////////////
//   eflowBinnedParameters   //
///////////////////////////////

const int eflowBinnedParameters::m_nShapeParams = 4;

eflowBinnedParameters::~eflowBinnedParameters() {
  int nEBins = m_bins.size();

  for (int i = 0; i < nEBins; i++) {

    int nEtaBins = m_bins[i].size();

    for (int j = 0; j < nEtaBins; j++)
      delete m_bins[i][j];
  }
}

int eflowBinnedParameters::nShapeParams()
{
  return m_nShapeParams;
}

void eflowBinnedParameters::initialise(const std::vector<double>& eBinBounds, const std::vector<double>& etaBinBounds, bool useAbsEta) {
  m_useAbsEta = useAbsEta;
  m_eBinBounds = eBinBounds;
  m_etaBinBounds = etaBinBounds;

  /* Create all the bins */
  int nEBins = getNumEBins();
  int nEtaBins = getNumEtaBins()-1;
  eflowParamEtaBin* dummy = 0;
  m_bins.assign(nEBins, std::vector<eflowParamEtaBin*>(nEtaBins, dummy));
  for (int iEBin = 0; iEBin < nEBins; iEBin++) {
    for (int iEtaBin = 0; iEtaBin < nEtaBins; iEtaBin++) {
      m_bins[iEBin][iEtaBin] = new eflowParamEtaBin(this);
    }
  }
}


void eflowBinnedParameters::getWeightedMeanBin(eflowFirstIntParameters& meanBin, const eflowFirstIntParameters* bin1, const eflowFirstIntParameters* bin2, double w1) const {
  double fudgeMean = w1 * bin1->fudgeMean() + (1.0 - w1) * bin2->fudgeMean();
  double fudgeStdDev = w1 * bin1->fudgeStdDev() + (1.0 - w1) * bin2->fudgeStdDev();
  std::vector<double> par(m_nShapeParams);
	
  meanBin.setFudgeMean(fudgeMean);
  meanBin.setFudgeStdDev(fudgeStdDev);
	
  for (int j = 0; j < eflowCalo::nRegions; j++) {
    eflowCaloENUM layer = (eflowCaloENUM)j;

    const std::vector<double>& par1 = bin1->getShapeParameters(layer);
    const std::vector<double>& par2 = bin2->getShapeParameters(layer);
	  
    for (int k = 0; k < m_nShapeParams; k++) {
      par[k] = w1 * par1[k] + (1.0 - w1) * par2[k];
    }

    meanBin.setShapeParameters(layer, par);
  }
}

eflowFirstIntENUM eflowBinnedParameters::adjustLFI(double e, double eta,
                                                   eflowFirstIntENUM j1st) const {
  int eBinIndex = getEBinIndex(e);
  //hard code this check - needs to be rechecked if ever remake e/p tables
  //if e.g track extrapolation slightly off can have inconsistent j1st and eta (e.g eta = 1.52, j1st = EMB2) and hence fudgeMean is zero
  if (0 == eBinIndex || 1 == eBinIndex) {
    if (fabs(eta) >= 1.0 && fabs(eta) < 1.1 && eflowFirstIntRegions::Tile == j1st) j1st =
        eflowFirstIntRegions::EMB3;

    if (fabs(eta) >= 1.2 && fabs(eta) < 1.3 && eflowFirstIntRegions::Tile == j1st) j1st =
        eflowFirstIntRegions::EMB3;

    if (fabs(eta) < 1.5 && eflowFirstIntRegions::EME1 == j1st) j1st = eflowFirstIntRegions::EMB1;

    if (fabs(eta) < 1.5 && eflowFirstIntRegions::EME2 == j1st) j1st = eflowFirstIntRegions::EMB2;

    if (fabs(eta) < 1.5 && eflowFirstIntRegions::EME3 == j1st) j1st = eflowFirstIntRegions::EMB3;

    if (fabs(eta) < 1.5 && eflowFirstIntRegions::HEC == j1st) j1st = eflowFirstIntRegions::Tile;

    if (fabs(eta) >= 1.5 && fabs(eta) < 1.6 && eflowFirstIntRegions::EMB3 == j1st) j1st =
        eflowFirstIntRegions::EME3;

    if (fabs(eta) >= 1.6 && eflowFirstIntRegions::EMB1 == j1st) j1st = eflowFirstIntRegions::EME1;

    if (fabs(eta) >= 1.6 && eflowFirstIntRegions::EMB2 == j1st) j1st = eflowFirstIntRegions::EME2;

    if (fabs(eta) >= 1.6 && eflowFirstIntRegions::EMB3 == j1st) j1st = eflowFirstIntRegions::EME3;

    if (fabs(eta) >= 1.6 && eflowFirstIntRegions::Tile == j1st) j1st = eflowFirstIntRegions::HEC;
  } else if (2 == eBinIndex) {
    if (fabs(eta) < 1.5 && eflowFirstIntRegions::EME1 == j1st) j1st = eflowFirstIntRegions::EMB1;

    if (fabs(eta) < 1.5 && eflowFirstIntRegions::EME2 == j1st) j1st = eflowFirstIntRegions::EMB2;

    if (fabs(eta) < 1.5 && eflowFirstIntRegions::EME3 == j1st) j1st = eflowFirstIntRegions::EMB3;

    if (fabs(eta) < 1.5 && eflowFirstIntRegions::HEC == j1st) j1st = eflowFirstIntRegions::Tile;

    if (fabs(eta) >= 1.5 && fabs(eta) < 1.6 && eflowFirstIntRegions::EMB2 == j1st) j1st =
        eflowFirstIntRegions::EME2;

    if (fabs(eta) >= 1.6 && eflowFirstIntRegions::EMB1 == j1st) j1st = eflowFirstIntRegions::EME1;

    if (fabs(eta) >= 1.6 && eflowFirstIntRegions::EMB2 == j1st) j1st = eflowFirstIntRegions::EME2;

    if (fabs(eta) >= 1.6 && eflowFirstIntRegions::EMB3 == j1st) j1st = eflowFirstIntRegions::EME3;

    if (fabs(eta) >= 1.6 && eflowFirstIntRegions::Tile == j1st) j1st = eflowFirstIntRegions::HEC;
  } else if (3 == eBinIndex) {
    if (fabs(eta) < 1.5 && eflowFirstIntRegions::EME1 == j1st) j1st = eflowFirstIntRegions::EMB1;

    if (fabs(eta) < 1.5 && eflowFirstIntRegions::EME2 == j1st) j1st = eflowFirstIntRegions::EMB2;

    if (fabs(eta) < 1.5 && eflowFirstIntRegions::EME3 == j1st) j1st = eflowFirstIntRegions::EMB3;

    if (fabs(eta) < 1.5 && eflowFirstIntRegions::HEC == j1st) j1st = eflowFirstIntRegions::Tile;

    if (fabs(eta) >= 1.5 && fabs(eta) < 1.6 && eflowFirstIntRegions::EMB2 == j1st) j1st =
        eflowFirstIntRegions::EME2;

    if (fabs(eta) >= 1.5 && fabs(eta) < 1.6 && eflowFirstIntRegions::EMB1 == j1st) j1st =
        eflowFirstIntRegions::EME1;

    if (fabs(eta) >= 1.6 && eflowFirstIntRegions::EMB1 == j1st) j1st = eflowFirstIntRegions::EME1;

    if (fabs(eta) >= 1.6 && eflowFirstIntRegions::EMB2 == j1st) j1st = eflowFirstIntRegions::EME2;

    if (fabs(eta) >= 1.6 && eflowFirstIntRegions::EMB3 == j1st) j1st = eflowFirstIntRegions::EME3;

    if (fabs(eta) >= 1.6 && eflowFirstIntRegions::Tile == j1st) j1st = eflowFirstIntRegions::HEC;
  } else if (4 == eBinIndex || 5 == eBinIndex) {
    if (fabs(eta) < 1.5 && eflowFirstIntRegions::EME1 == j1st) j1st = eflowFirstIntRegions::EMB1;

    if (fabs(eta) < 1.5 && eflowFirstIntRegions::EME2 == j1st) j1st = eflowFirstIntRegions::EMB2;

    if (fabs(eta) < 1.5 && eflowFirstIntRegions::EME3 == j1st) j1st = eflowFirstIntRegions::EMB3;

    if (fabs(eta) < 1.5 && eflowFirstIntRegions::HEC == j1st) j1st = eflowFirstIntRegions::Tile;

    if (fabs(eta) >= 1.5 && fabs(eta) < 1.6 && eflowFirstIntRegions::EMB1 == j1st) j1st =
        eflowFirstIntRegions::EME1;

    if (fabs(eta) >= 1.6 && eflowFirstIntRegions::EMB1 == j1st) j1st = eflowFirstIntRegions::EME1;

    if (fabs(eta) >= 1.6 && eflowFirstIntRegions::EMB2 == j1st) j1st = eflowFirstIntRegions::EME2;

    if (fabs(eta) >= 1.6 && eflowFirstIntRegions::EMB3 == j1st) j1st = eflowFirstIntRegions::EME3;

    if (fabs(eta) >= 1.6 && eflowFirstIntRegions::Tile == j1st) j1st = eflowFirstIntRegions::HEC;
  }

  return j1st;
}

bool eflowBinnedParameters::getOrdering(eflowCellSubtractionManager& subtMan, double e, double eta, eflowFirstIntENUM j1st) const {
  const eflowParamEtaBin* bin1 = 0;
  const eflowParamEtaBin* bin2 = 0;
  double w1 = -1.0;

  getInterpolation(&bin1, &bin2, w1, e, eta);
  if (bin1 && bin2) {

    eflowFirstIntENUM adjustedJ1st = adjustLFI(e, eta, j1st);
    const eflowFirstIntParameters* firstIntBin1 = bin1->getFirstIntBin(adjustedJ1st);
    const eflowFirstIntParameters* firstIntBin2 = bin2->getFirstIntBin(adjustedJ1st);
    if (firstIntBin1 && firstIntBin2) {

      /* Interpolate the firstInBins */
      eflowFirstIntParameters meanBin;
      getWeightedMeanBin(meanBin, firstIntBin1, firstIntBin2, w1);

      /* Interpolate the ring thicknesses */
      std::vector<double> ringThicknesses(eflowCalo::nRegions);
      for (int i = 0; i < eflowCalo::nRegions; i++) {
        ringThicknesses[i] = w1 * bin1->getRingThickness( (eflowCaloENUM)i ) + (1.0 - w1) * bin2->getRingThickness( (eflowCaloENUM)i );
      }

      subtMan.setParameters(meanBin, ringThicknesses);
      return true;
    }
  }

  return false;
}


void eflowBinnedParameters::setRingThickness(int energyBin, int etaBin, eflowCaloENUM ringNo, double thickness) {
  eflowParamEtaBin* pEtaBin = m_bins[energyBin][etaBin];
  if (pEtaBin) {
    pEtaBin->setRingThickness(ringNo, thickness);
  }
}

void eflowBinnedParameters::setFudgeMean(int energyBin, int etaBin, eflowFirstIntENUM j1st, double fudgeMean) {
  eflowParamEtaBin* pEtaBin = m_bins[energyBin][etaBin];
  if (pEtaBin) {
    eflowFirstIntParameters* j1stBin = pEtaBin->getFirstIntBin(j1st);
    if (j1stBin) j1stBin->setFudgeMean(fudgeMean);
  }
}

void eflowBinnedParameters::setFudgeStdDev(int energyBin, int etaBin, eflowFirstIntENUM j1st, double fudgeStdDev) {
  eflowParamEtaBin* pEtaBin = m_bins[energyBin][etaBin];
  if (pEtaBin) {
    eflowFirstIntParameters* j1stBin = pEtaBin->getFirstIntBin(j1st);
    if (j1stBin) j1stBin->setFudgeStdDev(fudgeStdDev);
  }
}

void eflowBinnedParameters::setShapeParam(int energyBin, int etaBin, eflowFirstIntENUM j1st, eflowCaloENUM layer, int paramNumber, double shapeParam) {
  eflowParamEtaBin* pEtaBin = m_bins[energyBin][etaBin];
  if (pEtaBin) {
    eflowFirstIntParameters* j1stBin = pEtaBin->getFirstIntBin(j1st);
    if (j1stBin) j1stBin->setShapeParameter(layer, paramNumber, shapeParam);
  }
}
