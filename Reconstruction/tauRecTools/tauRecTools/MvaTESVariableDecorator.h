/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLSDEV_MVATESVARIABLEDECORATOR_H
#define TAURECTOOLSDEV_MVATESVARIABLEDECORATOR_H

#include "tauRecTools/TauRecToolBase.h"
#include "tauRecTools/ITauVertexCorrection.h"

#include "xAODEventInfo/EventInfo.h"
#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgDataHandles/ReadDecorHandleKey.h"
#include "AsgTools/ToolHandle.h"


class MvaTESVariableDecorator
: public TauRecToolBase
{
 public:
  
  ASG_TOOL_CLASS2( MvaTESVariableDecorator, TauRecToolBase, ITauToolBase )
    
  MvaTESVariableDecorator(const std::string& name="MvaTESVariableDecorator");
  virtual ~MvaTESVariableDecorator();
    
  virtual StatusCode initialize() override;
  virtual StatusCode execute(xAOD::TauJet& xTau) const override;

 private:

  bool m_incShowerSubtr;

  SG::ReadDecorHandleKey<xAOD::EventInfo> m_aveIntPerXKey {this, 
      "AveIntPerXKey", 
      "EventInfo.AveIntPerXDecor",
      "Decoration for Average Interaction Per Crossing"};
  
  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainerKey {this,
      "Key_vertexInputContainer",
      "PrimaryVertices",
      "input vertex container key"};

  ToolHandle<ITauVertexCorrection> m_tauVertexCorrection { this, 
      "TauVertexCorrection", "TauVertexCorrection", "Tool to perform the vertex correction"};
};


#endif // TAURECTOOLSDEV_MVATESVARIABLEDECORATOR_H
