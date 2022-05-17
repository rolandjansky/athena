/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Author: Bill Balunas <bill.balunas@cern.ch>

#ifndef XAODPFLOW_FEHELPERS_H
#define XAODPFLOW_FEHELPERS_H 1

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

  xAOD::Type::ObjectType signalToXAODType(const xAOD::FlowElement& fe);
  
  class FillNeutralFlowElements {

    public:
      FillNeutralFlowElements();
      /** Function to add cluster moments onto FE */
      bool addMoment(const xAOD::CaloCluster::MomentType& momentType,                     
                     const std::string& attributeName,
                     const xAOD::CaloCluster& theCluster,
                     xAOD::FlowElement& theFE);

      void addSamplingEnergy(const xAOD::CaloCluster::CaloSample& sampling,
                    const std::string& attributeName,
                    const xAOD::CaloCluster& theCluster,
                    xAOD::FlowElement& theFE);

      void addStandardMoments(xAOD::FlowElement& theFE,const xAOD::CaloCluster& theCluster);
      void addStandardCalHitMoments(xAOD::FlowElement& theFE,const xAOD::CaloCluster& theCluster);
      void addStandardSamplingEnergies(xAOD::FlowElement& theFE,const xAOD::CaloCluster& theCluster);      
  };

}

#endif
