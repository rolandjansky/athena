/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTSEEDING_L1TRIGGERRESULTMAKER_H
#define HLTSEEDING_L1TRIGGERRESULTMAKER_H

// Local includes
#include "HLTSeeding/IRoIThresholdsTool.h"

// Trigger includes
#include "xAODTrigger/eFexEMRoIContainer.h"
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

  // Threshold pattern tools
  ToolHandleArray<IRoIThresholdsTool> m_thresholdPatternTools {
    this, "ThresholdPatternTools", {},
    "Tools decorating RoI containers with threshold patterns"
  };

  // Placeholder for other L1 xAOD outputs:
  // - CTP result
  // - L1Topo result
  // - the remaining Run-3 L1Calo RoIs
};

#endif // HLTSEEDING_L1TRIGGERRESULTMAKER_H
