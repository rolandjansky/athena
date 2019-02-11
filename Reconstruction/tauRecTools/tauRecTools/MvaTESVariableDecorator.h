/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLSDEV_MVATESVARIABLEDECORATOR_H
#define TAURECTOOLSDEV_MVATESVARIABLEDECORATOR_H

// tauRecTools include(s)
#include "tauRecTools/TauRecToolBase.h"

// xAOD include(s)
#include "xAODEventInfo/EventInfo.h"

class MvaTESVariableDecorator
: public TauRecToolBase
{
 public:
  
  ASG_TOOL_CLASS2( MvaTESVariableDecorator, TauRecToolBase, ITauToolBase )
    
  MvaTESVariableDecorator(const std::string& name="MvaTESVariableDecorator");
  virtual ~MvaTESVariableDecorator();
    
  StatusCode initialize();
  StatusCode eventInitialize();
  StatusCode execute(xAOD::TauJet& xTau);
  StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
  StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
  StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS; }
  StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
  StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
  StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }

  StatusCode eventFinalize();
  
 private:
  const xAOD::EventInfo* m_xEventInfo;  //!
  const xAOD::VertexContainer* m_xVertexContainer; //!
  int m_mu; //!
  int m_nVtxPU; //!
  bool m_emitVertexWarning=true; //!

  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexInputContainer{this,"Key_vertexInputContainer", "PrimaryVertices", "input vertex container key"};
};


#endif // TAURECTOOLSDEV_MVATESVARIABLEDECORATOR_H
