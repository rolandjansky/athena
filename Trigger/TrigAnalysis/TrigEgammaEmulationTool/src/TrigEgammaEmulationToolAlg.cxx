/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaEmulationToolAlg.h"
#include "GaudiKernel/Property.h"
#include "xAODTrigger/EmTauRoI.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "PATCore/TAccept.h"

namespace Trig {
TrigEgammaEmulationToolAlg::
TrigEgammaEmulationToolAlg(const std::string& name, ISvcLocator* pSvcLocator )
: ::AthAlgorithm( name, pSvcLocator ),
    m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_matchTool("Trig::TrigEgammaMatchingTool/TrigEgammaMatchingTool",this),
    m_emulationTool("Trig::TrigEgammaEmulationTool/TrigEgammaEmulationTool",this)
{
    declareProperty("TrigEgammaMatchingTool", m_matchTool);
    declareProperty("TrigEgammaEmulationTool",m_emulationTool);
    declareProperty("TriggerList", m_triggerList);
    declareProperty("perfTrigger", m_perfTrigger = "HLT_e0_perf_L1EM15");
    m_storeGate=nullptr;
}

TrigEgammaEmulationToolAlg::~TrigEgammaEmulationToolAlg() { }

StatusCode TrigEgammaEmulationToolAlg::initialize() {
  ATH_MSG_INFO("Initializing " << name() << "...");
  ATH_MSG_INFO("Retrieving tools...");
  if ((m_trigdec.retrieve()).isFailure()) {
    ATH_MSG_ERROR("Could not retrieve Trigger Decision Tool! Can't work");
    return StatusCode::FAILURE;
  }
  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Unable to locate Service StoreGateSvc" );
    return sc;
  }
  for (const auto &trigger : m_triggerList) {
    m_trigger_tdt[trigger]  = 0;
    m_trigger_emu1[trigger] = 0;
    m_trigger_emu2[trigger] = 0;
    m_trigger_emu3[trigger] = 0;
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaEmulationToolAlg::finalize() {
  ATH_MSG_INFO("Finalizing " << name() << "...");
  ATH_MSG_INFO("Trigger emulation summary");
  for (const auto &trigger : m_triggerList) {
    ATH_MSG_INFO("REGTEST: " << trigger << " " << m_trigger_tdt[trigger]
                                        << " " << m_trigger_emu1[trigger]
                                        << " " << m_trigger_emu2[trigger]
                                        << " " << m_trigger_emu3[trigger]);
  }
  std::ofstream ofile("HLTCounts.txt");
  if (!ofile.is_open()) return StatusCode::FAILURE;
  for (const auto &trigger : m_triggerList) {
    ofile << trigger << " " << m_trigger_tdt[trigger]
                            << " " << m_trigger_emu1[trigger]
                            << " " << m_trigger_emu2[trigger]
                            << " " << m_trigger_emu3[trigger] << "\n";
  }
  ofile.close();
  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaEmulationToolAlg::execute() {
  ATH_MSG_DEBUG("TrigEgammaEmulationTool::execute()");
  m_emulationTool->EventWiseContainer(); // Do not retrieve containers for every trigger, only once per event
  bool tdt = false, emu1 = false, emu2 = false, emu3 = false;
  for (const auto &trigger : m_triggerList) {
    // 2 different counts
    tdt = m_trigdec->isPassed(trigger);
    emu1 = m_emulationTool->isPassed(trigger);
    emu2 = m_emulationTool->isPassed(trigger, m_perfTrigger);
    emu3 = m_emulationTool->isPassed(trigger, trigger);
    ATH_MSG_DEBUG("Emulating " << trigger);
    ATH_MSG_DEBUG("TDT = " << tdt << "\t accept1 = " << emu1 << "\t accept2 = " << emu2 << "\t accept3 = " << emu3);
    m_trigger_tdt[trigger]  += tdt;
    m_trigger_emu1[trigger] += emu1;
    m_trigger_emu2[trigger] += emu2;
    m_trigger_emu3[trigger] += emu3;
  }
  return StatusCode::SUCCESS;
}

}
