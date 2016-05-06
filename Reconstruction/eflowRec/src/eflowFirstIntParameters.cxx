/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowFirstIntParameters.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson, R Duxfield (based on R.Duxfields Root package)
CREATED:  18th Aug, 2005

********************************************************************/

//Athena Headers
#include "eflowRec/eflowCaloRegions.h"
#include "eflowRec/eflowEEtaBinnedParameters.h"
#include "eflowRec/eflowFirstIntParameters.h"
#include <iostream>

const int eflowFirstIntParameters::m_nShapeParams = 4;

eflowFirstIntParameters::eflowFirstIntParameters() :
  m_parameters(eflowCalo::nRegions) {
  for (int i = 0; i < eflowCalo::nRegions; i++) {
    m_parameters[i].resize(m_nShapeParams);
    for (int j = 0; j < m_nShapeParams; j++) m_parameters[i][j] = 0.0;
  }
}

bool eflowFirstIntParameters::getWeightedParameters(const eflowFirstIntParameters* bin1, const eflowFirstIntParameters* bin2, const double w1) {
  if (!(bin1 && bin2)) { return false; }

  double fudgeMean = w1 * bin1->fudgeMean() + (1.0 - w1) * bin2->fudgeMean();
  double fudgeStdDev = w1 * bin1->fudgeStdDev() + (1.0 - w1) * bin2->fudgeStdDev();
  std::vector<double> par(m_nShapeParams);

  setFudgeMean(fudgeMean);
  setFudgeStdDev(fudgeStdDev);

  for (int j = 0; j < eflowCalo::nRegions; j++) {
    eflowCaloENUM layer = (eflowCaloENUM)j;

    const std::vector<double>& par1 = bin1->getShapeParameters(layer);
    const std::vector<double>& par2 = bin2->getShapeParameters(layer);

    for (int k = 0; k < m_nShapeParams; k++) {
      par[k] = w1 * par1[k] + (1.0 - w1) * par2[k];
    }

    setShapeParameters(layer, par);
  }

  return true;
}

void eflowFirstIntParameters::printM_Parameters() {
  for (int i = 0; i < eflowCalo::nRegions; i++)
    for (int j = 0; j < m_nShapeParams; j++)
      std::cout << "m_p[" << i << "][" << j << "] is " << m_parameters[i][j] << std::endl;
}
