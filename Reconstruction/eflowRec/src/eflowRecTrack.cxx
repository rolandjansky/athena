/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * eflowRecTrack.cxx
 *
 *  Created on: 30.09.2013
 *      Author: tlodd
 */

#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowDepthCalculator.h"

void eflowRecTrack::setCaloDepthArray(const double* depthArray) {
  if (!m_caloDepthArray.empty()){
    std::cout << "WARNING\tResetting caloDepthArray in eflowRecTrack!" << std::endl;
  }
  m_caloDepthArray.assign(depthArray, depthArray + eflowDepthCalculator::NDepth() + 1);
}
