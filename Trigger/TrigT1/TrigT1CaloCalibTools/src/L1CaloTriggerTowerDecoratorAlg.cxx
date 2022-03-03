/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


// TrigT1 common definitions
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

// Local include(s):
#include "TrigT1CaloCalibTools/L1CaloTriggerTowerDecoratorAlg.h"

// EDM include(s):
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

namespace LVL1 {

L1CaloTriggerTowerDecoratorAlg::L1CaloTriggerTowerDecoratorAlg(
    const std::string& name, ISvcLocator* svcLoc)
    : AthAlgorithm(name, svcLoc),
      m_caloCellEnergy(""),  // disabled by default
      m_caloCellET(""),      // disabled by default
      m_caloCellEnergyByLayer("CaloCellEnergyByLayer"),
      m_caloCellETByLayer("CaloCellETByLayer"),
      m_caloCellsQuality("CaloCellQuality"),
      m_caloCellEnergyByLayerByReceiver(""),  // disabled by default
      m_caloCellETByLayerByReceiver(""),       // disabled by default
      m_ttTools(this)
{
  declareProperty("DecorName_caloCellEnergy",
                  m_caloCellEnergy,  // disabled by default
                  "Decoration name - leave empty to disable");
  declareProperty("DecorName_caloCellET", m_caloCellET,  // disabled by default
                  "Decoration name - leave empty to disable");
  declareProperty("DecorName_caloCellEnergyByLayer", m_caloCellEnergyByLayer,
                  "Decoration name - leave empty to disable");
  declareProperty("DecorName_caloCellETByLayer", m_caloCellETByLayer,
                  "Decoration name - leave empty to disable");
  declareProperty("DecorName_caloCellQuality", m_caloCellsQuality,
                  "Decoration name - leave empty to disable");
  declareProperty(
      "DecorName_caloCellEnergyByLayerByReceiver",
      m_caloCellEnergyByLayerByReceiver,
      "Decoration name - leave empty to disable");  // disabled by default
  declareProperty(
      "DecorName_caloCellETByLayerByReceiver", m_caloCellETByLayerByReceiver,
      "Decoration name - leave empty to disable");  // disabled by default

  declareProperty("TriggerTowerTools", m_ttTools);
}

StatusCode L1CaloTriggerTowerDecoratorAlg::initialize() {
  ATH_MSG_INFO(
      "TrigT1CaloCalibTools/L1CaloTriggerTowerDecoratorAlg::initialize()");
  CHECK(m_ttTools.retrieve());

  if (!m_caloCellEnergy.empty()) {
    m_caloCellEnergyDecorator =
        std::make_unique<xAOD::TriggerTower::Decorator<float>>(m_caloCellEnergy);
  }

  if (!m_caloCellET.empty()) {
    m_caloCellETDecorator =
        std::make_unique<xAOD::TriggerTower::Decorator<float>>(m_caloCellET);
  }

  if (!m_caloCellsQuality.empty()) {
    m_caloCellsQualityDecorator =
        std::make_unique<xAOD::TriggerTower::Decorator<float>>(m_caloCellsQuality);
  }

  if (!m_caloCellEnergyByLayer.empty()) {
    m_caloCellEnergyByLayerDecorator =
        std::make_unique<xAOD::TriggerTower::Decorator<std::vector<float>>>(
            m_caloCellEnergyByLayer);
  }

  if (!m_caloCellETByLayer.empty()) {
    m_caloCellETByLayerDecorator =
        std::make_unique<xAOD::TriggerTower::Decorator<std::vector<float>>>(
            m_caloCellETByLayer);
  }

  if (!m_caloCellEnergyByLayerByReceiver.empty()) {
    m_caloCellEnergyByLayerByReceiverDecorator =
        std::make_unique<xAOD::TriggerTower::Decorator<std::vector<std::vector<float>>>>(
            m_caloCellEnergyByLayerByReceiver);
  }

  if (!m_caloCellETByLayerByReceiver.empty()) {
    m_caloCellETByLayerByReceiverDecorator =
        std::make_unique<xAOD::TriggerTower::Decorator<std::vector<std::vector<float>>>>(
            m_caloCellETByLayerByReceiver);
  }

  CHECK( m_triggerTowerContainerKey.initialize(SG::AllowEmpty) );

  // Return gracefully:
  return StatusCode::SUCCESS;
}

StatusCode L1CaloTriggerTowerDecoratorAlg::execute() {
  // use decorators to avoid the costly name -> auxid lookup

  // Shall I proceed?
  if (!m_triggerTowerContainerKey.empty()) {
    CHECK(m_ttTools->initCaloCells());

    const EventContext& ctx = Gaudi::Hive::currentContext();
    SG::ReadHandle<xAOD::TriggerTowerContainer> tts (m_triggerTowerContainerKey, ctx);
    for (const auto x : *tts) {
      if (m_caloCellEnergyDecorator)
        (*m_caloCellEnergyDecorator)(*x) = m_ttTools->caloCellsEnergy(*x);
      if (m_caloCellETDecorator)
        (*m_caloCellETDecorator)(*x) = m_ttTools->caloCellsET(*x);
      if (m_caloCellEnergyByLayerDecorator)
        (*m_caloCellEnergyByLayerDecorator)(*x) =
            m_ttTools->caloCellsEnergyByLayer(*x);
      if (m_caloCellETByLayerDecorator)
        (*m_caloCellETByLayerDecorator)(*x) = m_ttTools->caloCellsETByLayer(*x);
      if (m_caloCellsQualityDecorator)
        (*m_caloCellsQualityDecorator)(*x) = m_ttTools->caloCellsQuality(*x);
      if (m_caloCellEnergyByLayerByReceiverDecorator) {
        (*m_caloCellEnergyByLayerByReceiverDecorator)(*x) =
            m_ttTools->caloCellsEnergyByLayerByReceiver(*x);
      }
      if (m_caloCellETByLayerByReceiverDecorator) {
        (*m_caloCellETByLayerByReceiverDecorator)(*x) =
            m_ttTools->caloCellsETByLayerByReceiver(*x);
      }
    }
  }  // Trigger towers present in Storegate

  // Return gracefully:
  return StatusCode::SUCCESS;
}
}
