// JetOriginCorrectionTool.h  -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JetMomentTools_JetOriginCorrectionTool_H
#define JetMomentTools_JetOriginCorrectionTool_H

////////////////////////////////////////////////
/// \file JetOriginCorrectionTool.h
/// \class JetOriginCorrectionTool
/// 
/// This tool computes the constituent scale 4-vector of the jet 
/// relative to a PV vertex and stores it in as an attribute  
/// of the jet. 
/// This tool DOES NOT change the scale of the jet, just adds an attribute.
///
/// This tool is a wrapper : it only calls functions defined in JetUtils/JetOriginHelpers.h
///
/// Properties:
///  - VertexContainer: Name of the vertex container. The 0th vertex is used to compute the correction
///  - OriginCorrectedName: Name of the output attribute
////////////////////////////////////////////////////////////

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetDecorator.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"

class JetOriginCorrectionTool : public asg::AsgTool,
                                virtual public IJetDecorator {
  ASG_TOOL_CLASS(JetOriginCorrectionTool, IJetDecorator)
    
public:

  /// Constructor from tool name.
  JetOriginCorrectionTool(const std::string& myname);

  /// Inherited method to modify a jet container. Compute the origin-corrected
  /// momentum and put it in the jets
  StatusCode decorate(const xAOD::JetContainer& jet) const override;
  virtual StatusCode initialize() override;

 protected:
  
  Gaudi::Property<std::string> m_correctionName{this, "OriginCorrectedName", "JetOriginConstitScaleMomentum", "Origin corrected name"};
  Gaudi::Property<std::string> m_jetContainerName{this, "JetContainer", "", "SG key for the input jet container"};
  Gaudi::Property<bool> m_onlyAssignPV{this, "OnlyAssignPV", false, "Only write out PV information"};

private:

  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainer_key{this, "VertexContainer", "PrimaryVertices", "Primary vertex container name"};
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo_key{this, "EventInfoName", "EventInfo", "Event info name"};

  Gaudi::Property<std::string> m_scaleMomentumName{this, "jetScaleMomentName", "JetOriginConstitScaleMomentum", "SG key for JetScaleMomentum components"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_scaleMomentumPtKey{this, "JetScaleMomentumPtName", "pt", "SG suffix for output JetScaleMomentum (pt) decorator"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_scaleMomentumPhiKey{this, "JetScaleMomentumPhiName", "phi", "SG suffix for output JetScaleMomentum (phi) decorator"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_scaleMomentumEtaKey{this, "JetScaleMomentumEtaName", "eta", "SG suffix for output JetScaleMomentum (eta) decorator"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_scaleMomentumMKey{this, "JetScaleMomentumMName", "m", "SG suffix for output JetScaleMomentum (mass) decorator"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_originVertexKey{this, "OriginVertexName", "OriginVertex", "SG key for output OriginVertex decorator"};
};

#endif
