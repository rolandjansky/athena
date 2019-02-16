/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak

#include <MuonTruthOverlay/MuonSimDataOverlay.h>

#include <StoreGate/ReadHandle.h>
#include <StoreGate/WriteHandle.h>



MuonSimDataOverlay::MuonSimDataOverlay(const std::string &name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator) {}

StatusCode MuonSimDataOverlay::initialize()
{
  ATH_MSG_DEBUG("Initializing...");

  if (m_signalInputKey.key().empty()) {
    ATH_MSG_ERROR("Missing signal input SDO key.");
    return StatusCode::FAILURE;
  }

  if (m_outputKey.key().empty()) {
    ATH_MSG_ERROR("Missing output SDO key.");
    return StatusCode::FAILURE;
  }

  // Check and initialize keys
  ATH_CHECK( m_bkgInputKey.initialize(!m_bkgInputKey.key().empty()) );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_bkgInputKey.key());
  ATH_CHECK( m_signalInputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_signalInputKey.key());
  ATH_CHECK( m_outputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputKey.key());

  return StatusCode::SUCCESS;
}


StatusCode MuonSimDataOverlay::execute()
{
  ATH_MSG_DEBUG("execute() begin");

  ATH_MSG_VERBOSE("Retrieving input containers");
  
  const MuonSimDataCollection *bkgContainerPtr = nullptr;
  if (!m_bkgInputKey.key().empty()) {
    SG::ReadHandle<MuonSimDataCollection> bkgContainer(m_bkgInputKey);
    if (!bkgContainer.isValid()) {   
      ATH_MSG_ERROR("Could not get background MuonSimDataCollection container " << bkgContainer.name() << " from store " << bkgContainer.store());
      return StatusCode::FAILURE;
    }
    bkgContainerPtr = bkgContainer.cptr();
    
    ATH_MSG_DEBUG("Found background MuonSimDataCollection container " << bkgContainer.name() << " in store " << bkgContainer.store());
  }

  SG::ReadHandle<MuonSimDataCollection> signalContainer(m_signalInputKey);
  if (!signalContainer.isValid()) {
    ATH_MSG_ERROR("Could not get signal MuonSimDataCollection container " << signalContainer.name() << " from store " << signalContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found signal MuonSimDataCollection container " << signalContainer.name() << " in store " << signalContainer.store());

  // Creating output RDO container
  SG::WriteHandle<MuonSimDataCollection> outputContainer(m_outputKey);
  ATH_CHECK(outputContainer.record(std::make_unique<MuonSimDataCollection>()));
  if (!outputContainer.isValid()) {
    ATH_MSG_ERROR("Could not record output MuonSimDataCollection container " << outputContainer.name() << " to store " << outputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output MuonSimDataCollection container " << outputContainer.name() << " in store " << outputContainer.store());


  // Copy over signal SDO
  ATH_MSG_VERBOSE("Inserting signal SDO, size = " << signalContainer->size());
  outputContainer->insert(signalContainer->begin(), signalContainer->end());

  // Merge with background
  if (!m_bkgInputKey.key().empty()) {
    ATH_MSG_VERBOSE("Inserting background SDO, size = " << bkgContainerPtr->size());

    for (const auto &entry : *bkgContainerPtr) {
      auto it = outputContainer->find(entry.first);
      if (it != outputContainer->end()) {
        if (it->second.getTime() < entry.second.getTime()) {
          // We take the earlier in time
          continue;
        }

        ATH_MSG_VERBOSE("Replacing signal SDO, time = " << it->second.getTime() << ", "
        << "with background SDO, time = " << entry.second.getTime());

        MuonSimData tempSDO(entry.second.getdeposits(), 0);
        tempSDO.setPosition(entry.second.globalPosition());
        tempSDO.setTime(entry.second.getTime());

        it->second = tempSDO;
      } else {
        outputContainer->insert(entry);
      }
    }
  }

  ATH_MSG_VERBOSE("Output SDO size " << outputContainer->size());

  ATH_MSG_DEBUG("execute() end");
  return StatusCode::SUCCESS;
}
