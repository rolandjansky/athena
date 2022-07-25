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
#include "StoreGate/DataHandle.h"
#include "StoreGate/ReadDecorHandleKey.h"

#include "xAODPFlow/FlowElementContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODCore/AuxContainerBase.h"

class JetPFlowSelectionAlg : public EL::AnaReentrantAlgorithm { 

public:

  JetPFlowSelectionAlg(const std::string & n, ISvcLocator* l) : EL::AnaReentrantAlgorithm(n,l) {}
  using EL::AnaReentrantAlgorithm::AnaReentrantAlgorithm;

  /// Athena algorithm's Hooks
  StatusCode  initialize() override;
  StatusCode  execute(const EventContext& ctx) const override;

private:

  bool checkLeptonLinks(const std::vector < ElementLink< xAOD::ElectronContainer > >& chargedFE_ElectronLinks,
			const std::vector < ElementLink< xAOD::MuonContainer > >& chargedFE_MuonLinks) const;
  
  Gaudi::Property<std::string> m_electronID{this,"electronID","LHMedium","Select electron ID"};

  SG::ReadHandleKey<xAOD::FlowElementContainer> m_ChargedPFlowContainerKey = {this, "ChargedPFlowInputContainer", "", "The input Charged PFlow Objects"};
  SG::ReadHandleKey<xAOD::FlowElementContainer> m_NeutralPFlowContainerKey = {this, "NeutralPFlowInputContainer", "", "The input Neutral PFlow Objects"};
  
  SG::WriteHandleKey<xAOD::FlowElementContainer> m_outputChargedPFlowHandleKey= {this, "ChargedPFlowOutputContainer", "GlobalPFlowChargedParticleFlowObjects", "The output filtered Charged PFlow Objects"};
  SG::WriteHandleKey<xAOD::FlowElementContainer> m_outputNeutralPFlowHandleKey= {this, "NeutralPFlowOutputContainer", "GlobalPFlowNeutralParticleFlowObjects", "The output filtered Neutral PFlow Objects"};

  SG::ReadDecorHandleKey<xAOD::FlowElementContainer> m_chargedFEElectronsReadDecorKey {this, "ChargedFEElectronsReadDecorKey", "JetETMissChargedParticleFlowObjects.FE_ElectronLinks", "Key for links from charged FE to electrons"};
  SG::ReadDecorHandleKey<xAOD::FlowElementContainer> m_chargedFEMuonsReadDecorKey {this, "ChargedFEMuonsReadDecorKey", "JetETMissChargedParticleFlowObjects.FE_MuonLinks", "Key for links from charged FE to muons"};

  // these ReadDecorHandleKeys are introduced to make sure PFTauFlowElementAssoc runs before JetPFlowSelectionAlg
  // otherwise FEs can be simultaneously decorated in PFTauFlowElementAssoc and deep-copied in JetPFlowSelectionAlg, causing std::bad_array_new_length exceptions
  SG::ReadDecorHandleKey<xAOD::FlowElementContainer> m_chargedFETausReadDecorKey {this, "ChargedFETausReadDecorKey", "JetETMissChargedParticleFlowObjects.FE_TauLinks", "Key for links from charged FE to taus"};
  SG::ReadDecorHandleKey<xAOD::FlowElementContainer> m_neutralFETausReadDecorKey {this, "NeutralFETausReadDecorKey", "JetETMissNeutralParticleFlowObjects.FE_TauLinks", "Key for links from neutral FE to taus"};

};

#endif
