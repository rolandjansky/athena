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


eflowCellSubtractionManager::eflowCellSubtractionManager() : m_ringThickness(eflowCalo::nRegions) {}

eflowCellSubtractionManager::eflowCellSubtractionManager(const eflowCellSubtractionManager& other) :
    eflowFirstInt(other.m_fudgeMean, other.m_fudgeStdDev),
    m_ringThickness(other.m_ringThickness), m_rankMap(other.m_rankMap) { }

double eflowCellSubtractionManager::ringMeanRank(const vector<double>& p, double rMin, double rMax) const {

  double x1 = 0.0;
  double x2 = 0.0;

  if (0 != p[1] && 0 != p[3]){

    x1 = exp(-rMin / p[1]) - exp(-rMax / p[1]);
    x2 = exp(-rMin / p[3]) - exp(-rMax / p[3]);

  }

  return (p[0] * p[1] * x1 + p[2] * p[3] * x2) / (rMax - rMin); // hard coded 4-param model for now
}

void eflowCellSubtractionManager::setParameters(const eflowFirstIntParameters& param, const vector<double>& ringThickness, double rMax, double range)
{
  double p0PrevMax = 1.0e12; // something big!
  double weightMax = 0.0;

  m_fudgeMean = param.fudgeMean();
  m_fudgeStdDev = param.fudgeStdDev();
  m_ringThickness = ringThickness;

  m_rankMap.clear();

  // pick layer with highest p[0] (normalisation) and add weights down to the weight[0] / range
  // pick layer with next highest p[0] and add weights down to the same value
  // etc, etc


  for (int i = 0; i < eflowCalo::nRegions; i++) {

    double p0Max = -1.0;
    eflowCaloENUM layer = eflowCalo::Unknown;

    // find layer with next highest p0

    for (int j = 0; j < eflowCalo::nRegions; j++) {

      double p0 = param.getShapeParameter( (eflowCaloENUM)j, 0 );

      if (p0 < p0PrevMax && p0 > p0Max) {
	layer = (eflowCaloENUM)j;
	p0Max = p0;
      }
    }

    if (eflowCalo::Unknown != layer){
      p0PrevMax = p0Max;

      if (0 == i) weightMax = ringMeanRank(param.getShapeParameters(layer), 0.0, ringThickness[layer]);  // absolute highest

      // add weights to map (map does the ordering)

      const int nRings = (int)(rMax / ringThickness[layer]);

      double weight;
      
      for (int j = 0; j < nRings; j++) {
	
	weight = ringMeanRank(param.getShapeParameters(layer), j*ringThickness[layer], (j+1)*ringThickness[layer]);
	
	if (weight < weightMax / range || 0 == weight) {
	  break;                    // only add weights within a factor (range) of weightMax (say 10^6)
	}
	else {
	  RingId id(layer, j);
	  m_rankMap[1.0/weight] = id;
	}
      }
    }//protection against layer = 999 which will not give well defined ringThickness since ringThicness does not have 999 elements.

  }

}
