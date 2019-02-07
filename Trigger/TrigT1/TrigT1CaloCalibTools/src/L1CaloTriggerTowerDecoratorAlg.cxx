/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L1CaloTriggerTowerDecoratorAlg.cxx 728363 2016-03-08 12:45:29Z amazurov $

// TrigT1 common definitions
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

// Local include(s):
#include "TrigT1CaloCalibTools/L1CaloTriggerTowerDecoratorAlg.h"

// EDM include(s):
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

namespace {
xAOD::TriggerTower::Decorator<float>* caloCellEnergyDecorator;
xAOD::TriggerTower::Decorator<float>* caloCellETDecorator;
xAOD::TriggerTower::Decorator<float>* caloCellsQualityDecorator;
xAOD::TriggerTower::Decorator<std::vector<float>>*
    caloCellEnergyByLayerDecorator;
xAOD::TriggerTower::Decorator<std::vector<float>>* caloCellETByLayerDecorator;
xAOD::TriggerTower::Decorator<std::vector<std::vector<float>>>*
    caloCellEnergyByLayerByReceiverDecorator;
xAOD::TriggerTower::Decorator<std::vector<std::vector<float>>>*
    caloCellETByLayerByReceiverDecorator;
}

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
    caloCellEnergyDecorator =
        new xAOD::TriggerTower::Decorator<float>(m_caloCellEnergy);
  }

  if (!m_caloCellET.empty()) {
    caloCellETDecorator =
        new xAOD::TriggerTower::Decorator<float>(m_caloCellET);
  }

  if (!m_caloCellsQuality.empty()) {
    caloCellsQualityDecorator =
        new xAOD::TriggerTower::Decorator<float>(m_caloCellsQuality);
  }

  if (!m_caloCellEnergyByLayer.empty()) {
    caloCellEnergyByLayerDecorator =
        new xAOD::TriggerTower::Decorator<std::vector<float>>(
            m_caloCellEnergyByLayer);
  }

  if (!m_caloCellETByLayer.empty()) {
    caloCellETByLayerDecorator =
        new xAOD::TriggerTower::Decorator<std::vector<float>>(
            m_caloCellETByLayer);
  }

  if (!m_caloCellEnergyByLayerByReceiver.empty()) {
    caloCellEnergyByLayerByReceiverDecorator =
        new xAOD::TriggerTower::Decorator<std::vector<std::vector<float>>>(
            m_caloCellEnergyByLayerByReceiver);
  }

  if (!m_caloCellETByLayerByReceiver.empty()) {
    caloCellETByLayerByReceiverDecorator =
        new xAOD::TriggerTower::Decorator<std::vector<std::vector<float>>>(
            m_caloCellETByLayerByReceiver);
  }

  CHECK( m_triggerTowerContainerKey.initialize(SG::AllowEmpty) );

  // Return gracefully:
  return StatusCode::SUCCESS;
}

StatusCode L1CaloTriggerTowerDecoratorAlg::finalize() {
  ATH_MSG_INFO(
      "TrigT1CaloCalibTools/L1CaloTriggerTowerDecoratorAlg::finalize()");

  delete caloCellEnergyDecorator;
  delete caloCellETDecorator;
  delete caloCellsQualityDecorator;
  delete caloCellEnergyByLayerDecorator;
  delete caloCellETByLayerDecorator;
  delete caloCellEnergyByLayerByReceiverDecorator;
  delete caloCellETByLayerByReceiverDecorator;

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
      if (caloCellEnergyDecorator)
        (*caloCellEnergyDecorator)(*x) = m_ttTools->caloCellsEnergy(*x);
      if (caloCellETDecorator)
        (*caloCellETDecorator)(*x) = m_ttTools->caloCellsET(*x);
      if (caloCellEnergyByLayerDecorator)
        (*caloCellEnergyByLayerDecorator)(*x) =
            m_ttTools->caloCellsEnergyByLayer(*x);
      if (caloCellETByLayerDecorator)
        (*caloCellETByLayerDecorator)(*x) = m_ttTools->caloCellsETByLayer(*x);
      if (caloCellsQualityDecorator)
        (*caloCellsQualityDecorator)(*x) = m_ttTools->caloCellsQuality(*x);
      if (caloCellEnergyByLayerByReceiverDecorator) {
        (*caloCellEnergyByLayerByReceiverDecorator)(*x) =
            m_ttTools->caloCellsEnergyByLayerByReceiver(*x);
      }
      if (caloCellETByLayerByReceiverDecorator) {
        (*caloCellETByLayerByReceiverDecorator)(*x) =
            m_ttTools->caloCellsETByLayerByReceiver(*x);
      }
    }
  }  // Trigger towers present in Storegate

  // Return gracefully:
  return StatusCode::SUCCESS;
}
}
