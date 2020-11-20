/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_TAUVERTEXEDCLUSTERDECORATOR_H
#define TAURECTOOLS_TAUVERTEXEDCLUSTERDECORATOR_H

#include "tauRecTools/TauRecToolBase.h"

#include "xAODJet/Jet.h"

/** 
 * @brief Decorate CaloVertexedTopoCluster to the tau candidate.
 * 
 * @author Xiaozhong Huang <xiaozhong.huang@cern.ch>
 */

class TauVertexedClusterDecorator : public TauRecToolBase {

public:

  ASG_TOOL_CLASS2(TauVertexedClusterDecorator, TauRecToolBase, ITauToolBase)
  
  TauVertexedClusterDecorator(const std::string& name);
 
  virtual ~TauVertexedClusterDecorator() = default;

  virtual StatusCode initialize() override;

  virtual StatusCode execute(xAOD::TauJet& tau) const override;
  
private:
  //* Get the vertex used in jet reconstruction */
  const xAOD::Vertex* getJetVertex(const xAOD::Jet& jet) const;

  /// Name of the seed jet
  std::string m_seedJet;  
  
  /// Switch of the tau vertex correction
  bool m_doVertexCorrection;
  
  /// Switch of the jet vertex correction
  bool m_doJetVertexCorrection;

  /// Calibration state of cluster
  xAOD::CaloCluster::State m_clusterState; //!
};

#endif //TAURECTOOLS_TAUVERTEXEDCLUSTERDECORATOR_H
