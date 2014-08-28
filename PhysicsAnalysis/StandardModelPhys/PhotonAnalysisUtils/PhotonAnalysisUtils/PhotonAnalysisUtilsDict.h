/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHOTONANALYSISUTILS_DICT_H
#define PHOTONANALYSISUTILS_DICT_H

#include "PhotonAnalysisUtils/IDiphotonVertexID.h"
#include "PhotonAnalysisUtils/IComputePointingQuantities.h"

// #include "TLorentzVector.h"
#include<vector>

namespace PhotonAnalysisUtilsDict {
  struct tmp {
    std::vector<const Trk::VxCandidate*> m_1;
    //std::vector<TLorentzVector> m_2;
 };
}

#endif
