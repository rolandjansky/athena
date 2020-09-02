/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Local includes:
#include "TrigT1ResultByteStream/RecRoIBResultByteStreamTool.h"

// Trigger includes:
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Interfaces/RecJetRoI.h"
#include "TrigT1Result/RecRoIBResult.h"

// TDAQ includes:
#include "eformat/SourceIdentifier.h"

// System includes
#include <sstream>

using DataType = OFFLINE_FRAGMENTS_NAMESPACE::PointerType;
using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

/**
 * The constructor takes care of correctly constructing the base class and
 * declaring the tool's interface to the framework.
 */
RecRoIBResultByteStreamTool::RecRoIBResultByteStreamTool( const std::string& type,
                                                          const std::string& name,
                                                          const IInterface* parent )
  : AthAlgTool( type, name, parent ) {}

/**
 * @brief Initialise the tool
 *
 * Retrieve services and thresholds configuration, fill the vector of configured ROB IDs and print the module IDs
 * to debug message stream
 */
StatusCode RecRoIBResultByteStreamTool::initialize() {
  ATH_MSG_DEBUG("Initialising RecRoIBResultByteStreamTool");
  // ---------------------------------------------------------------------------
  // Initialise service handles
  // ---------------------------------------------------------------------------
  ATH_CHECK(m_configSvc.retrieve());
  ATH_CHECK(m_rpcRoISvc.retrieve());
  ATH_CHECK(m_tgcRoISvc.retrieve());

  // ---------------------------------------------------------------------------
  // Load the threshold vectors
  // ---------------------------------------------------------------------------
  const std::vector<TrigConf::TriggerThreshold*> &thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
  for (TrigConf::TriggerThreshold* threshold : thresholds) {
    switch (threshold->ttype()) {
      case TrigConf::L1DataDef::TriggerType::MUON:
        m_muonConfig.push_back(threshold);
        break;
      case TrigConf::L1DataDef::TriggerType::EM:
      case TrigConf::L1DataDef::TriggerType::TAU:
        m_emtauConfig.push_back(threshold);
        break;
      case TrigConf::L1DataDef::TriggerType::JET:
      case TrigConf::L1DataDef::TriggerType::JF:
      case TrigConf::L1DataDef::TriggerType::JB:
        m_jetConfig.push_back(threshold);
        break;
      default:
        break;
    }
  }

  // ---------------------------------------------------------------------------
  // Fill the vector of configured ROB IDs and print the module IDs to debug message stream
  // ---------------------------------------------------------------------------
  std::vector<eformat::helper::SourceIdentifier> configuredROBSIDs;
  std::ostringstream str;
  ATH_MSG_DEBUG("Configured module IDs for:");

  // MUCTPI
  configuredROBSIDs.emplace_back(eformat::TDAQ_MUON_CTP_INTERFACE, m_muCTPIModuleID);
  ATH_MSG_DEBUG("   muCTPi                               = 0x" << MSG::hex << m_muCTPIModuleID.value() << MSG::dec);

  // Jet/Energy
  str.str("");
  for (const uint16_t module_id : m_jetModuleID) {
    configuredROBSIDs.emplace_back(eformat::TDAQ_CALO_JET_PROC_ROI, module_id);
    str << "0x" << std::hex << module_id << std::dec << " ";
  }
  ATH_MSG_DEBUG("   Calorimeter Jet/Energy Processor RoI = " << str.str());

  // EM/Tau
  str.str("");
  for (const uint16_t module_id : m_emModuleID) {
    configuredROBSIDs.emplace_back(eformat::TDAQ_CALO_CLUSTER_PROC_ROI, module_id);
    str << "0x" << std::hex << module_id << std::dec << " ";
  }
  ATH_MSG_DEBUG("   Calorimeter Cluster Processor RoI    = " << str.str());

  // Fill the ROB ID vector
  for (const auto& sid : configuredROBSIDs) {
    m_configuredROBIds.push_back( sid.code() );
  }

  return StatusCode::SUCCESS;
}

/**
 * @brief Conversion from eformat::ROBFragments to RIO
 *
 * This will be called from the decoder algorithm RecRoIBResultByteStreamDecoderAlg when implemented
 */
StatusCode RecRoIBResultByteStreamTool::convert(const std::vector<const ROBFragment*>& vrobf,
                                             ROIB::RecRoIBResult& resultToFill) const {
  ATH_MSG_DEBUG("Executing convert() from ROBFragments to RIO");

  // Create containers for objects to be filled
   std::vector<LVL1::RecMuonRoI>  muonContent;
   std::vector<LVL1::RecEmTauRoI> emTauContent;
   std::vector<LVL1::RecJetRoI>   jetContent;

  // Loop over ROB fragments
  for (const ROBFragment* p_robf : vrobf) {
    const ROBFragment& robf = *p_robf;
    eformat::helper::SourceIdentifier rodSID(robf.rod_source_id());
    eformat::SubDetector rodSubdetId = rodSID.subdetector_id();
    uint16_t rodModuleId = rodSID.module_id();

    switch (rodSubdetId) {
      // -----------------------------------------------------------------------
      // MUCTPI
      // -----------------------------------------------------------------------
      case eformat::TDAQ_MUON_CTP_INTERFACE: {
        // Check if the current ROD module ID matches the configured MuCTPI module ID
        if (rodModuleId != m_muCTPIModuleID) continue;
        ATH_MSG_DEBUG("   Found MuCTPI ROD with source ID " << MSG::hex << rodSID.code() << MSG::dec);

        // Create RecMuonRoI object
        DataType it_data;
        robf.rod_data(it_data);
        const uint32_t ndata = robf.rod_ndata();
        for (uint32_t i=0; i<ndata; ++i, ++it_data) {
          muonContent.emplace_back(static_cast<uint32_t>(*it_data),
                                   m_rpcRoISvc.get(),
                                   m_tgcRoISvc.get(),
                                   &m_muonConfig);
        }
        break;
      }

      // -----------------------------------------------------------------------
      // Jet/Energy
      // -----------------------------------------------------------------------
      case eformat::TDAQ_CALO_JET_PROC_ROI: {
        // Check if the current ROD module ID matches the configured Jet/Energy module IDs
        auto it = std::find(m_jetModuleID.begin(), m_jetModuleID.end(), rodModuleId);
        if (it == m_jetModuleID.end()) continue;
        ATH_MSG_DEBUG("   Found Jet/Energy ROD with source ID " << MSG::hex << rodSID.code() << MSG::dec);

        // Create RecJetRoI object
        DataType it_data;
        robf.rod_data(it_data);
        const uint32_t ndata = robf.rod_ndata();
        for (uint32_t i=0; i<ndata; ++i, ++it_data) {
          emTauContent.emplace_back(static_cast<uint32_t>(*it_data), &m_jetConfig);
        }
        break;
      }

      // -----------------------------------------------------------------------
      // EM/Tau
      // -----------------------------------------------------------------------
      case eformat::TDAQ_CALO_CLUSTER_PROC_ROI: {
        // Check if the current ROD module ID matches the configured EM/Tau module IDs
        auto it = std::find(m_emModuleID.begin(), m_emModuleID.end(), rodModuleId);
        if (it == m_emModuleID.end()) continue;
        ATH_MSG_DEBUG("   Found EM/Tau ROD with source ID " << MSG::hex << rodSID.code() << MSG::dec);

        // Create RecEmTauRoI object
        DataType it_data;
        robf.rod_data(it_data);
        const uint32_t ndata = robf.rod_ndata();
        for (uint32_t i=0; i<ndata; ++i, ++it_data) {
          emTauContent.emplace_back(static_cast<uint32_t>(*it_data), &m_emtauConfig);
        }
        break;
      }

      default: {
        ATH_MSG_DEBUG("Skipping ROD with SubDetID " << rodSID.human_detector());
        break;
      }
    }
  }
  ATH_MSG_DEBUG("Building RecRoIBResult with " << emTauContent.size() << " EM/Tau RoIs, "
                << muonContent.size() << " Muon RoIs, " << jetContent.size() << " Jet/Energy RoIs");
  resultToFill = ROIB::RecRoIBResult(std::move(emTauContent), std::move(muonContent), std::move(jetContent));
  return StatusCode::SUCCESS;
}
