/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTSEEDING_L1TRIGGERRESULTMAKER_H
#define HLTSEEDING_L1TRIGGERRESULTMAKER_H

// Local includes
#include "HLTSeeding/IRoIThresholdsTool.h"

// Trigger includes
#include "xAODTrigger/eFexEMRoIContainer.h"
#include "xAODTrigger/eFexTauRoIContainer.h"
#include "xAODTrigger/jFexTauRoIContainer.h"
#include "xAODTrigger/jFexSRJetRoIContainer.h"
#include "xAODTrigger/jFexLRJetRoIContainer.h"
#include "xAODTrigger/gFexJetRoIContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTrigger/TrigCompositeContainer.h"

// Athena includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

/** @class L1TriggerResultMaker
 *  @brief Algorithm creating L1TriggerResult and linking the relevant L1 xAOD collections to it
 **/
class L1TriggerResultMaker : public AthReentrantAlgorithm {
public:
  /// Standard constructor
  L1TriggerResultMaker(const std::string& name, ISvcLocator* svcLoc);

  // ------------------------- AthReentrantAlgorithm methods -------------------
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& eventContext) const override;

private:
  // ------------------------- Properties --------------------------------------
  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_l1TriggerResultWHKey {
    this, "L1TriggerResultWHKey", "L1TriggerResult",
    "Key of the output L1 Trigger Result"};

  // Muon RoIs
  SG::ReadHandleKey<xAOD::MuonRoIContainer> m_muRoIKey {
    this, "MuRoIKey", "LVL1MuonRoIs",
    "Key of the muon RoI container to be linked to L1 Trigger Result"};

  // eFex EM RoIs
  SG::ReadHandleKey<xAOD::eFexEMRoIContainer> m_eFexEMRoIKey {
    this, "eFexEMRoIKey", "L1_eEMRoI",
    "Key of the eFex EM RoI container to be linked to L1 Trigger Result"};

  // eFex Tau RoIs
  SG::ReadHandleKey<xAOD::eFexTauRoIContainer> m_eFexTauRoIKey {
    this, "eFexTauRoIKey", "L1_eTauRoI",
    "Key of the eFex Tau RoI container to be linked to L1 Trigger Result"};

  // jFex Tau RoIs
  SG::ReadHandleKey<xAOD::jFexTauRoIContainer> m_jFexTauRoIKey {
    this, "jFexTauRoIKey", "L1_jFexTauRoI",
    "Key of the jFex Tau RoI container to be linked to L1 Trigger Result"};

  // jFex small-R Jet RoIs
  SG::ReadHandleKey<xAOD::jFexSRJetRoIContainer> m_jFexSRJetRoIKey {
    this, "jFexSRJetRoIKey", "L1_jFexSRJetRoI",
    "Key of the jFex small-R Jet RoI container to be linked to L1 Trigger Result"};

  // jFex large-R Jet RoIs
  SG::ReadHandleKey<xAOD::jFexLRJetRoIContainer> m_jFexLRJetRoIKey {
    this, "jFexLRJetRoIKey", "L1_jFexLRJetRoI",
    "Key of the jFex large-R Jet RoI container to be linked to L1 Trigger Result"};

  // gFex small-R Jet RoIs
  SG::ReadHandleKey<xAOD::gFexJetRoIContainer> m_gFexSRJetRoIKey {
    this, "gFexSRJetRoIKey", "L1_gFexSRJetRoI",
    "Key of the gFex small-R Jet RoI container to be linked to L1 Trigger Result"};

  // gFex large-R Jet RoIs
  SG::ReadHandleKey<xAOD::gFexJetRoIContainer> m_gFexLRJetRoIKey {
    this, "gFexLRJetRoIKey", "L1_gFexLRJetRoI",
    "Key of the gFex large-R Jet RoI container to be linked to L1 Trigger Result"};

  // Key of the cTau container to create (if empty, cTau creation is disabled)
  SG::WriteHandleKey<xAOD::eFexTauRoIContainer> m_cTauRoIKey {
    this, "cTauRoIKey", "L1_cTauRoI",
    "Key of the cTau RoI container to be created (contains copies of eTaus matched to jTaus)"};

  // Key of the cTau decoration linking eFexTau to the matching jFexTau
  SG::WriteDecorHandleKey<xAOD::eFexTauRoIContainer> m_cjTauLinkKey {
    this, "cjTauLinkKey", "L1_cTauRoI.jTauLink",
    "Decoration for the link from eTau to the matching jTau"};

  // Threshold pattern tools
  ToolHandleArray<IRoIThresholdsTool> m_thresholdPatternTools {
    this, "ThresholdPatternTools", {},
    "Tools decorating RoI containers with threshold patterns"
  };

  // Placeholder for other L1 xAOD outputs:
  // - CTP result
  // - L1Topo result
  // - the remaining Run-3 L1Calo RoIs

  // ------------------------- Helper methods ----------------------------------
  /// Create the combined Tau container matching eTau to jTau
  StatusCode createCombinedTauRoIs(xAOD::TrigComposite& l1tr, const EventContext& eventContext) const;
};

#endif // HLTSEEDING_L1TRIGGERRESULTMAKER_H
