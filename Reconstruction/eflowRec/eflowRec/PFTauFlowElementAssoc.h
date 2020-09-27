/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
 * PFTauFlowElementAssoc.h
 * Header file for class PFTauFlowElementAssoc
 *
 * Created on: 24.09.20
 *     Author: L. Miller
 */

#ifndef PFTAUFLOWELEMENTASSOC_H
#define PFTAUFLOWELEMENTASSOC_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/WriteDecorHandle.h"

#include "xAODPFlow/FlowElementContainer.h"
#include "xAODTau/TauJetContainer.h"

/**
This is the algorithm, which inherits from AthReentrantAlgorithm, that adds element links
between FlowElements (FEs) and Tau objects. Charged FEs are associated with core tau
tracks and neutral FEs are associated with tau clusters.
Decorations are added to the TauJet container consisting of a vector of ElementLinks
to the charged and neutral FlowElement container.
Similarly, decorations are added to the charged and neutral FlowElement containers
(JetETMissChargedFlowElements and JetETMissNeutralFlowElements) which contain a
vector of ElementLinks to the associated TauJets.
*/

class PFTauFlowElementAssoc : public AthReentrantAlgorithm {

public:
  using AthReentrantAlgorithm::AthReentrantAlgorithm;

  PFTauFlowElementAssoc(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~PFTauFlowElementAssoc();

  virtual StatusCode initialize();
  virtual StatusCode execute(const EventContext &ctx) const;
  virtual StatusCode finalize();

private:

  // ReadHandle for the tau jets
  SG::ReadHandleKey<xAOD::TauJetContainer> m_tauJetReadHandleKey{this,"TauJetContainer","TauJets","ReadHandleKey for TauJet Container"};
  // ReadHandles for FlowElements
  SG::ReadHandleKey<xAOD::FlowElementContainer> m_neutralFEReadHandleKey{this,"JetETMissNeutralFlowElementContainer","JetETMissNeutralFlowElements","ReadHandleKey for neutral FlowElements"};
  SG::ReadHandleKey<xAOD::FlowElementContainer> m_chargedFEReadHandleKey{this,"JetETMissChargedFlowElementContainer","JetETMissChargedFlowElements","ReadHandleKey for charged FlowElements"};

  /** The write key for adding Neutral Flow Element links to the taus */
  SG::WriteDecorHandleKey<xAOD::TauJetContainer> m_tauNeutralFEWriteDecorKey;
  /** The write key for adding Charged Flow Element links to the taus */
  SG::WriteDecorHandleKey<xAOD::TauJetContainer> m_tauChargedFEWriteDecorKey;
  /** The write key for adding tau element links to the Neutral Flow Elements */
  SG::WriteDecorHandleKey<xAOD::FlowElementContainer> m_neutralFETauWriteDecorKey;
  /** The write key for adding tau element links to the Charged Flow Elements */
  SG::WriteDecorHandleKey<xAOD::FlowElementContainer> m_chargedFETauWriteDecorKey;

};

#endif // PFTAUFLOWELEMENTASSOC_H
