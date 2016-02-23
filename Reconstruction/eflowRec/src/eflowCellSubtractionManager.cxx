/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowCellSubtractionManager.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson, R Duxfield (based on R.Duxfields Root package)
CREATED:  18th Aug, 2005

 ********************************************************************/

//Athena Headers
#include "eflowRec/eflowCellSubtractionManager.h"
#include "eflowRec/eflowFirstIntParameters.h"

#include <cmath>
#include <vector>

using std::vector;

double eflowCellSubtractionManager::m_rMax = 0.75;
double eflowCellSubtractionManager::m_weightRange = 1.0e6;


eflowCellSubtractionManager::eflowCellSubtractionManager() : m_ringThickness(eflowCalo::nRegions) {}

eflowCellSubtractionManager::eflowCellSubtractionManager(const eflowCellSubtractionManager& other) :
  eflowFirstInt(other.m_fudgeMean, other.m_fudgeStdDev),
  m_ringThickness(other.m_ringThickness), m_rankMap(other.m_rankMap) { }

void eflowCellSubtractionManager::operator=(const eflowCellSubtractionManager& other){
  this->setFudgeMean(other.fudgeMean());
  this->setFudgeStdDev(other.fudgeStdDev());
  m_ringThickness= other.m_ringThickness;
  m_rankMap = other.m_rankMap;
}


double eflowCellSubtractionManager::ringMeanRank(const vector<double>& p, double rMin, double rMax) const {

  double x1 = 0.0;
  double x2 = 0.0;

  if (0 != p[1] && 0 != p[3]){
    x1 = exp(-rMin / p[1]) - exp(-rMax / p[1]);
    x2 = exp(-rMin / p[3]) - exp(-rMax / p[3]);
  }

  return (p[0] * p[1] * x1 + p[2] * p[3] * x2) / (rMax - rMin); // hard coded 4-param model for now
}

void eflowCellSubtractionManager::setParameters(const eflowFirstIntParameters& param, const vector<double>& ringThickness) {
  /* Prepare a vector of calo layers which is ordered by the p0 parameter */

  /* First prepare a list of all layers (which are not eflowCalo::Unknown i.e. 999) */
  std::list<eflowCaloENUM> allLayers;
  for (int iLayer = 0; iLayer < eflowCalo::nRegions; iLayer++) {
    if (iLayer != eflowCalo::Unknown){
      allLayers.push_back((eflowCaloENUM)iLayer);
    }
  }
  /* Loop over the list to find the layer with highest p0.
   * Attach it to the end of the ordered vector and remove it from the list.
   * Repeat until the list is empty */
  std::vector<eflowCaloENUM> orderedLayers;
  while (!allLayers.empty()){
    std::list<eflowCaloENUM>::iterator itLayer = allLayers.begin();
    std::list<eflowCaloENUM>::iterator itMaxP0Layer = itLayer;
    double p0Max = param.getShapeParameter( *itMaxP0Layer, 0 );
    /* Get current highest p0-layer */
    for (; itLayer != allLayers.end(); ++itLayer) {
      double p0 = param.getShapeParameter( *itLayer, 0 );
      if (p0 > p0Max) {
        itMaxP0Layer = itLayer;
        p0Max = p0;
      }
    }
    /* Attach to vector and remove from list */
    orderedLayers.push_back(*itMaxP0Layer);
    allLayers.erase(itMaxP0Layer);
  }

  /* Set e/p and ring thicknesses */
  m_fudgeMean = param.fudgeMean();
  m_fudgeStdDev = param.fudgeStdDev();
  m_ringThickness = ringThickness;

  /* Populate the ring/rank map */
  m_rankMap.clear();
  double weight;
  double weightMax = ringMeanRank(param.getShapeParameters(orderedLayers[0]), 0.0, ringThickness[orderedLayers[0]]);
  for (unsigned int i = 0; i < orderedLayers.size(); i++) {
    eflowCaloENUM layer = orderedLayers[i];

    // add weights to map (map does the ordering)
    const int nRings = (int)(m_rMax / ringThickness[layer]);

    for (int iRing = 0; iRing < nRings; iRing++) {
      weight = ringMeanRank(param.getShapeParameters(layer), iRing*ringThickness[layer], (iRing+1)*ringThickness[layer]);
      if (weight == 0 || weight < weightMax/m_weightRange) {
        break;                    // only add weights within a factor (range) of weightMax (say 10^6)
      } else {
        RingId id(layer, iRing);
        m_rankMap[1.0/weight] = id;
      }
    }

  }

}
