// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_TRIGT1_TRIGT1CALOXAODCALIBTOOLS_DECORATETRIGGERTOWERSALG_H
#define TRIGGER_TRIGT1_TRIGT1CALOXAODCALIBTOOLS_DECORATETRIGGERTOWERSALG_H

// Gaudi/Athena include(s):
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AsgTools/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"

// Local include(s):
#include "TrigT1CaloCalibToolInterfaces/IL1CaloxAODOfflineTriggerTowerTools.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include <memory>

namespace LVL1 {
class L1CaloTriggerTowerDecoratorAlg : public AthAlgorithm {
 public:
  L1CaloTriggerTowerDecoratorAlg(const std::string& name, ISvcLocator* svcLoc);

  /// Function initialising the algorithm
  virtual StatusCode initialize() override;
  /// Function executing the algorithm
  virtual StatusCode execute() override;

 private:
  SG::ReadHandleKey<xAOD::TriggerTowerContainer> m_triggerTowerContainerKey
  { this, "sgKey_TriggerTowers", LVL1::TrigT1CaloDefs::xAODTriggerTowerLocation, "" };
  std::string m_sgKey_TriggerTowers;

  /// Decoration strings (leave empty to disable the decoration)
  std::string m_caloCellEnergy;
  std::string m_caloCellET;
  std::string m_caloCellEnergyByLayer;
  std::string m_caloCellETByLayer;
  std::string m_caloCellsQuality;
  std::string m_caloCellEnergyByLayerByReceiver;
  std::string m_caloCellETByLayerByReceiver;

  ToolHandle<LVL1::IL1CaloxAODOfflineTriggerTowerTools> m_ttTools;

  std::unique_ptr<xAOD::TriggerTower::Decorator<float>> m_caloCellEnergyDecorator;
  std::unique_ptr<xAOD::TriggerTower::Decorator<float>> m_caloCellETDecorator;
  std::unique_ptr<xAOD::TriggerTower::Decorator<float>> m_caloCellsQualityDecorator;
  std::unique_ptr<xAOD::TriggerTower::Decorator<std::vector<float>>> m_caloCellEnergyByLayerDecorator;
  std::unique_ptr<xAOD::TriggerTower::Decorator<std::vector<float>>> m_caloCellETByLayerDecorator;
  std::unique_ptr<xAOD::TriggerTower::Decorator<std::vector<std::vector<float>>>> m_caloCellEnergyByLayerByReceiverDecorator;
  std::unique_ptr<xAOD::TriggerTower::Decorator<std::vector<std::vector<float>>>> m_caloCellETByLayerByReceiverDecorator;
};
}
#endif
