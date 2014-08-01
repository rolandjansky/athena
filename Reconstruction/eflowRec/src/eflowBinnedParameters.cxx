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
  m_parentSystem(0),
  m_ringThicknesses(eflowCalo::nRegions)
{
  m_eMin = eflowBaseEEtaBinSystem::getErrorReturnValue();
  m_eMax = eflowBaseEEtaBinSystem::getErrorReturnValue();
  m_etaMin = eflowBaseEEtaBinSystem::getErrorReturnValue();
  m_etaMax = eflowBaseEEtaBinSystem::getErrorReturnValue();

  int nSubtRegions = eflowFirstIntRegions::nRegions;
  m_firstIntBins.resize(nSubtRegions);
  for (int i = 0; i < nSubtRegions; i++) m_firstIntBins[i] = new eflowFirstIntParameters;
}


eflowParamEtaBin::eflowParamEtaBin(const eflowBinnedParameters* parentSystem, double eMin, double eMax, double etaMin, double etaMax) :
  eflowBaseInterpolatedEtaBin::eflowBaseInterpolatedEtaBin(eMin, eMax, etaMin, etaMax),
  m_parentSystem(parentSystem),
  m_ringThicknesses(eflowCalo::nRegions)
{
  int nSubtRegions = eflowFirstIntRegions::nRegions;
  m_firstIntBins.resize(nSubtRegions);
  for (int i = 0; i < nSubtRegions; i++) m_firstIntBins[i] = new eflowFirstIntParameters;
}


eflowParamEtaBin::~eflowParamEtaBin()
{
  int n = m_firstIntBins.size();
  for (int i = 0; i < n; i++) delete m_firstIntBins[i];
}


void eflowParamEtaBin::setToUndefined()
{
}


const eflowFirstIntParameters* eflowParamEtaBin::getFirstIntBin(eflowFirstIntENUM j1st) const 
{
  return (eflowFirstIntRegions::Unknown != j1st) ? m_firstIntBins[j1st] : 0;
}


eflowFirstIntParameters* eflowParamEtaBin::getFirstIntBin(eflowFirstIntENUM j1st)
{
  return (eflowFirstIntRegions::Unknown != j1st) ? m_firstIntBins[j1st] : 0;
}


bool eflowParamEtaBin::getOrdering(eflowCellSubtractionManager& subtMan, eflowFirstIntENUM j1st) const
{
  const eflowFirstIntParameters* paramsPtr = getFirstIntBin(j1st);
  
  if (paramsPtr) {
    subtMan.setParameters( *paramsPtr, m_ringThicknesses, m_parentSystem->getRMax(), m_parentSystem->getWeightRange() );
    return true;
  }
  else {
    subtMan.clear();
    return false;
  }
}




///////////////////////////////
//   eflowBinnedParameters   //
///////////////////////////////

const int eflowBinnedParameters::m_nShapeParams = 4;


int eflowBinnedParameters::nShapeParams()
{
  return m_nShapeParams;
}


void eflowBinnedParameters::access()
{
  int nEBins = getNumEBins();
  int nEtaBins = getNumEtaBins();
  int nBar = eflowCalo::nRegions;
  int nFirstInt = eflowFirstIntRegions::nRegions;
  
  for (int eBin = 0; eBin < nEBins; eBin++){
    for (int etaBin = 0; etaBin < nEtaBins; etaBin++){

      eflowParamEtaBin* eEtaBin = dynamic_cast<eflowParamEtaBin*>(m_bins[eBin][etaBin]);

      if (eEtaBin) {

	for (int i = 0; i < nBar; i++) {
	  eflowCaloENUM layer = (eflowCaloENUM)i;
	  std::cout << "m_ringThickness[eBin][etaBin][i] is " << eEtaBin->getRingThickness(layer) << " for bins of " << eBin << ", " << etaBin << " and " << i << std::endl;
	}

	for (int i = 0; i < nFirstInt; i++) {
	  
	  eflowFirstIntENUM j1st = (eflowFirstIntENUM)i;
	  eflowFirstIntParameters* j1stBin = dynamic_cast<eflowFirstIntParameters*>( eEtaBin->getFirstIntBin(j1st) );

	  if (j1stBin) {      
	    j1stBin->printFudge();
	    j1stBin->printM_P();
	  }
	}
      }

    }
  }
}


eflowBaseEtaBin* eflowBinnedParameters::makeNewEtaBin(int eBinIndex, int etaBinIndex) const
{
  return new eflowParamEtaBin( this, getEBinEnergyMin(eBinIndex), getEBinEnergyMax(eBinIndex), getEtaBinEtaMin(etaBinIndex), getEtaBinEtaMax(etaBinIndex) );
}


void eflowBinnedParameters::getWeightedMeanBin(eflowBaseInterpolatedEtaBin& meanBin, const eflowBaseEtaBin* bin1, const eflowBaseEtaBin* bin2, double w1) const
{
  eflowParamEtaBin* pMeanBin = dynamic_cast<eflowParamEtaBin*>(&meanBin);

  if (pMeanBin) {

    pMeanBin->setParent(this);

    const eflowParamEtaBin* pBin1 = dynamic_cast<const eflowParamEtaBin*>(bin1);
    const eflowParamEtaBin* pBin2 = dynamic_cast<const eflowParamEtaBin*>(bin2);
    
    if (pBin1 && pBin2) {
	
      std::vector<double> ringThicknesses(eflowCalo::nRegions);
      std::vector<double> par(m_nShapeParams);
 
      for (int i = 0; i < eflowCalo::nRegions; i++)
	ringThicknesses[i] = w1 * pBin1->getRingThickness( (eflowCaloENUM)i ) + (1.0 - w1) * pBin2->getRingThickness( (eflowCaloENUM)i );
      pMeanBin->setRingThicknesses(ringThicknesses);
      
      for (int i = 0; i < eflowFirstIntRegions::nRegions; i++) {

	eflowFirstIntENUM j1st = (eflowFirstIntENUM)i;

	eflowFirstIntParameters* pJ1stMeanBin = pMeanBin->getFirstIntBin(j1st);
	const eflowFirstIntParameters* pJ1stBin1 = pBin1->getFirstIntBin(j1st);
	const eflowFirstIntParameters* pJ1stBin2 = pBin2->getFirstIntBin(j1st);
	
	getWeightedMeanBin(*pJ1stMeanBin, pJ1stBin1, pJ1stBin2, w1);
      }

    }
  }

}


void eflowBinnedParameters::getWeightedMeanBin(eflowFirstIntParameters& meanBin, const eflowFirstIntParameters* bin1, const eflowFirstIntParameters* bin2, double w1) const
{
  double fudgeMean = w1 * bin1->fudgeMean() + (1.0 - w1) * bin2->fudgeMean();
  double fudgeStdDev = w1 * bin1->fudgeStdDev() + (1.0 - w1) * bin2->fudgeStdDev();
  std::vector<double> par(m_nShapeParams);
	
  meanBin.setFudgeMean(fudgeMean);
  meanBin.setFudgeStdDev(fudgeStdDev);
	
  for (int j = 0; j < eflowCalo::nRegions; j++) {
	  
    eflowCaloENUM layer = (eflowCaloENUM)j;

    const std::vector<double>& par1 = bin1->getShapeParameters(layer);
    const std::vector<double>& par2 = bin2->getShapeParameters(layer);
	  
    for (int k = 0; k < m_nShapeParams; k++)
      par[k] = w1 * par1[k] + (1.0 - w1) * par2[k];

    meanBin.setShapeParameters(layer, par);
  }
}


bool eflowBinnedParameters::getOrdering(eflowCellSubtractionManager& subtMan, double e, double eta, eflowFirstIntENUM j1st) const
{
  const eflowBaseEtaBin* bin1 = 0;
  const eflowBaseEtaBin* bin2 = 0;
  double w1 = -1.0;

  getInterpolation(&bin1, &bin2, w1, e, eta);

  const eflowParamEtaBin* paramBin1 = dynamic_cast<const eflowParamEtaBin*>(bin1);
  const eflowParamEtaBin* paramBin2 = dynamic_cast<const eflowParamEtaBin*>(bin2);

  if (paramBin1 && paramBin2) {
    
    std::vector<double> ringThicknesses(eflowCalo::nRegions);

    for (int i = 0; i < eflowCalo::nRegions; i++)
      ringThicknesses[i] = w1 * paramBin1->getRingThickness( (eflowCaloENUM)i ) + (1.0 - w1) * paramBin2->getRingThickness( (eflowCaloENUM)i );

    int eBinIndex = getEBinIndex(e);
    
    //hard code this check - needs to be rechecked if ever remake e/p tables
    //if e.g track extrapolation slightly off can have inconsistent j1st and eta (e.g eta = 1.52, j1st = EMB2) and hence fudgeMean is zero

    if (0 == eBinIndex || 1 == eBinIndex){

      if (fabs(eta) >= 1.0 && fabs(eta) < 1.1 && eflowFirstIntRegions::Tile == j1st) j1st = eflowFirstIntRegions::EMB3;
      if (fabs(eta) >= 1.2 && fabs(eta) < 1.3 && eflowFirstIntRegions::Tile == j1st) j1st = eflowFirstIntRegions::EMB3;

      if (fabs(eta) < 1.5 && eflowFirstIntRegions::EME1 == j1st) j1st = eflowFirstIntRegions::EMB1;
      if (fabs(eta) < 1.5 && eflowFirstIntRegions::EME2 == j1st) j1st = eflowFirstIntRegions::EMB2;
      if (fabs(eta) < 1.5 && eflowFirstIntRegions::EME3 == j1st) j1st = eflowFirstIntRegions::EMB3;
      if (fabs(eta) < 1.5 && eflowFirstIntRegions::HEC == j1st) j1st = eflowFirstIntRegions::Tile;

      if (fabs(eta) >= 1.5 && fabs(eta) < 1.6 && eflowFirstIntRegions::EMB3 == j1st ) j1st = eflowFirstIntRegions::EME3;
      
      if (fabs(eta) >= 1.6 && eflowFirstIntRegions::EMB1 == j1st) j1st = eflowFirstIntRegions::EME1;
      if (fabs(eta) >= 1.6 && eflowFirstIntRegions::EMB2 == j1st) j1st = eflowFirstIntRegions::EME2;
      if (fabs(eta) >= 1.6 && eflowFirstIntRegions::EMB3 == j1st) j1st = eflowFirstIntRegions::EME3;
      if (fabs(eta) >= 1.6 && eflowFirstIntRegions::Tile == j1st) j1st = eflowFirstIntRegions::HEC;
    }
    else if (2 == eBinIndex){
      if (fabs(eta) < 1.5 && eflowFirstIntRegions::EME1 == j1st) j1st = eflowFirstIntRegions::EMB1;
      if (fabs(eta) < 1.5 && eflowFirstIntRegions::EME2 == j1st) j1st = eflowFirstIntRegions::EMB2;
      if (fabs(eta) < 1.5 && eflowFirstIntRegions::EME3 == j1st) j1st = eflowFirstIntRegions::EMB3;
      if (fabs(eta) < 1.5 && eflowFirstIntRegions::HEC == j1st) j1st = eflowFirstIntRegions::Tile;

      if (fabs(eta) >= 1.5 && fabs(eta) < 1.6 && eflowFirstIntRegions::EMB2 == j1st ) j1st = eflowFirstIntRegions::EME2;
      
      if (fabs(eta) >= 1.6 && eflowFirstIntRegions::EMB1 == j1st) j1st = eflowFirstIntRegions::EME1;
      if (fabs(eta) >= 1.6 && eflowFirstIntRegions::EMB2 == j1st) j1st = eflowFirstIntRegions::EME2;
      if (fabs(eta) >= 1.6 && eflowFirstIntRegions::EMB3 == j1st) j1st = eflowFirstIntRegions::EME3;
      if (fabs(eta) >= 1.6 && eflowFirstIntRegions::Tile == j1st) j1st = eflowFirstIntRegions::HEC;
    }
    else if (3 == eBinIndex){
      if (fabs(eta) < 1.5 && eflowFirstIntRegions::EME1 == j1st) j1st = eflowFirstIntRegions::EMB1;
      if (fabs(eta) < 1.5 && eflowFirstIntRegions::EME2 == j1st) j1st = eflowFirstIntRegions::EMB2;
      if (fabs(eta) < 1.5 && eflowFirstIntRegions::EME3 == j1st) j1st = eflowFirstIntRegions::EMB3;
      if (fabs(eta) < 1.5 && eflowFirstIntRegions::HEC == j1st) j1st = eflowFirstIntRegions::Tile;

      if (fabs(eta) >= 1.5 && fabs(eta) < 1.6 && eflowFirstIntRegions::EMB2 == j1st ) j1st = eflowFirstIntRegions::EME2;
      if (fabs(eta) >= 1.5 && fabs(eta) < 1.6 && eflowFirstIntRegions::EMB1 == j1st ) j1st = eflowFirstIntRegions::EME1;

      if (fabs(eta) >= 1.6 && eflowFirstIntRegions::EMB1 == j1st) j1st = eflowFirstIntRegions::EME1;
      if (fabs(eta) >= 1.6 && eflowFirstIntRegions::EMB2 == j1st) j1st = eflowFirstIntRegions::EME2;
      if (fabs(eta) >= 1.6 && eflowFirstIntRegions::EMB3 == j1st) j1st = eflowFirstIntRegions::EME3;
      if (fabs(eta) >= 1.6 && eflowFirstIntRegions::Tile == j1st) j1st = eflowFirstIntRegions::HEC;
    }
    else if (4 == eBinIndex || 5 == eBinIndex){
      if (fabs(eta) < 1.5 && eflowFirstIntRegions::EME1 == j1st) j1st = eflowFirstIntRegions::EMB1;
      if (fabs(eta) < 1.5 && eflowFirstIntRegions::EME2 == j1st) j1st = eflowFirstIntRegions::EMB2;
      if (fabs(eta) < 1.5 && eflowFirstIntRegions::EME3 == j1st) j1st = eflowFirstIntRegions::EMB3;
      if (fabs(eta) < 1.5 && eflowFirstIntRegions::HEC == j1st) j1st = eflowFirstIntRegions::Tile;

      if (fabs(eta) >= 1.5 && fabs(eta) < 1.6 && eflowFirstIntRegions::EMB1 == j1st ) j1st = eflowFirstIntRegions::EME1;
      
      if (fabs(eta) >= 1.6 && eflowFirstIntRegions::EMB1 == j1st) j1st = eflowFirstIntRegions::EME1;
      if (fabs(eta) >= 1.6 && eflowFirstIntRegions::EMB2 == j1st) j1st = eflowFirstIntRegions::EME2;
      if (fabs(eta) >= 1.6 && eflowFirstIntRegions::EMB3 == j1st) j1st = eflowFirstIntRegions::EME3;
      if (fabs(eta) >= 1.6 && eflowFirstIntRegions::Tile == j1st) j1st = eflowFirstIntRegions::HEC;
    }

    const eflowFirstIntParameters* j1st1 = paramBin1->getFirstIntBin(j1st);
    const eflowFirstIntParameters* j1st2 = paramBin2->getFirstIntBin(j1st);
    
    if (j1st1 && j1st2) {

      eflowFirstIntParameters meanBin;
      getWeightedMeanBin(meanBin, j1st1, j1st2, w1);
      subtMan.setParameters(meanBin, ringThicknesses, m_rMax, m_weightRange);
      return true;
    }
  }

  return false;
}


void eflowBinnedParameters::setRingThickness(int energyBin, int etaBin, eflowCaloENUM ringNo, double thickness)
{
  eflowParamEtaBin* pEtaBin = dynamic_cast<eflowParamEtaBin*>( m_bins[energyBin][etaBin] );
  if (pEtaBin) pEtaBin->setRingThickness(ringNo, thickness);
}


void eflowBinnedParameters::setFudgeMean(int energyBin, int etaBin, eflowFirstIntENUM j1st, double fudgeMean)
{
  eflowParamEtaBin* pEtaBin = dynamic_cast<eflowParamEtaBin*>( m_bins[energyBin][etaBin] );
  
  if (pEtaBin) {
    eflowFirstIntParameters* j1stBin = pEtaBin->getFirstIntBin(j1st);
    if (j1stBin) j1stBin->setFudgeMean(fudgeMean);
  }
}


void eflowBinnedParameters::setFudgeStdDev(int energyBin, int etaBin, eflowFirstIntENUM j1st, double fudgeStdDev)
{
  eflowParamEtaBin* pEtaBin = dynamic_cast<eflowParamEtaBin*>( m_bins[energyBin][etaBin] );
  
  if (pEtaBin) {
    eflowFirstIntParameters* j1stBin = pEtaBin->getFirstIntBin(j1st);
    if (j1stBin) j1stBin->setFudgeStdDev(fudgeStdDev);
  }
}


void eflowBinnedParameters::setShapeParam(int energyBin, int etaBin, eflowFirstIntENUM j1st, eflowCaloENUM layer, int paramNumber, double shapeParam)
{
  eflowParamEtaBin* pEtaBin = dynamic_cast<eflowParamEtaBin*>( m_bins[energyBin][etaBin] );
  
  if (pEtaBin) {
    eflowFirstIntParameters* j1stBin = pEtaBin->getFirstIntBin(j1st);
    if (j1stBin) j1stBin->setShapeParameter(layer, paramNumber, shapeParam);
  }
}

void eflowBinnedParameters::setShapeParam(int energyBin, int etaBin, eflowFirstIntENUM j1st, eflowCaloENUM layer, std::vector<double> shapeParam){

  eflowParamEtaBin* pEtaBin = dynamic_cast<eflowParamEtaBin*>( m_bins[energyBin][etaBin] );

  if (pEtaBin){
    eflowFirstIntParameters* j1stBin = pEtaBin->getFirstIntBin(j1st);
    if (j1stBin) j1stBin->setShapeParameters(layer,  shapeParam);
  }

}
