/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

  /// Name of the seed jet
  std::string m_seedJet;  
  
  /// Calibration state of cluster
  xAOD::CaloCluster::State m_clusterState; //!
};

#endif //TAURECTOOLS_TAUVERTEXEDCLUSTERDECORATOR_H
