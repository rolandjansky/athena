/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Author: Bill Balunas <bill.balunas@cern.ch>

#ifndef PFLOWUTILS_FEHELPERS_H
#define PFLOWUTILS_FEHELPERS_H 1

#include "xAODPFlow/FlowElement.h"
#include "xAODTracking/Vertex.h"
#include "xAODCaloEvent/CaloCluster.h"

namespace FEHelpers {

  TLorentzVector getVertexCorrectedFourVec(const xAOD::FlowElement& fe, const xAOD::Vertex& vertexToCorrectTo);
  TLorentzVector getVertexCorrectedFourVec(const xAOD::FlowElement& fe, const TVector3& vertexToCorrectTo);
  void vertexCorrectTheFourVector(const xAOD::FlowElement& fe, const TVector3& vertexToCorrectTo, TLorentzVector& theFourVector);

  bool getClusterMoment(const xAOD::FlowElement& fe, xAOD::CaloCluster::MomentType momentType, float& value);
  std::vector<float> getEnergiesPerSampling(const xAOD::FlowElement& fe);

  std::string getClusterMomentName(xAOD::CaloCluster::MomentType momentType);

}

#endif