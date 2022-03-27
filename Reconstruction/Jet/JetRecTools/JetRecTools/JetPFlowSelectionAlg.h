// this is a -*- C++ -*- file
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////
/// \class JetPFlowSelectionAlg
///
/// Algorithm creates a filtered collection of PFlow Objects not matched to electrons/muons.
///

#ifndef JetPFlowSelectionAlg_H
#define JetPFlowSelectionAlg_H

#include "AnaAlgorithm/AnaReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

#include "xAODPFlow/FlowElementContainer.h"
#include "xAODCore/AuxContainerBase.h"


class IJetExecuteTool;

class JetPFlowSelectionAlg : public EL::AnaReentrantAlgorithm { 

public:

  JetPFlowSelectionAlg(const std::string & n, ISvcLocator* l) : EL::AnaReentrantAlgorithm(n,l) {}
  using EL::AnaReentrantAlgorithm::AnaReentrantAlgorithm;

  /// Athena algorithm's Hooks
  StatusCode  initialize() override;
  StatusCode  execute(const EventContext& ctx) const override;

private:

  bool checkLeptonLinks(const xAOD::FlowElement* fe) const;
  
  Gaudi::Property<std::string> m_electronID{this,"electronID","LHMedium","Select electron ID"};

  SG::ReadHandleKey<xAOD::FlowElementContainer> m_ChargedPFlowContainerKey = {this, "ChargedPFlowInputContainer", "", "The input Charged PFlow Objects"};
  SG::ReadHandleKey<xAOD::FlowElementContainer> m_NeutralPFlowContainerKey = {this, "NeutralPFlowInputContainer", "", "The input Neutral PFlow Objects"};
  
  SG::WriteHandleKey<xAOD::FlowElementContainer> m_outputChargedPFlowHandleKey= {this, "ChargedPFlowOutputContainer", "GlobalPFlowChargedParticleFlowObjects", "The output filtered Charged PFlow Objects"};
  SG::WriteHandleKey<xAOD::FlowElementContainer> m_outputNeutralPFlowHandleKey= {this, "NeutralPFlowOutputContainer", "GlobalPFlowNeutralParticleFlowObjects", "The output filtered Neutral PFlow Objects"};

};

#endif
