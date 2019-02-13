/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak

#include <MuonTruthOverlay/CscSimDataOverlay.h>

#include <StoreGate/ReadHandle.h>
#include <StoreGate/WriteHandle.h>



CscSimDataOverlay::CscSimDataOverlay(const std::string &name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator) {}

StatusCode CscSimDataOverlay::initialize()
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


StatusCode CscSimDataOverlay::execute()
{
  ATH_MSG_DEBUG("execute() begin");

  ATH_MSG_VERBOSE("Retrieving input containers");
  
  const CscSimDataCollection *bkgContainerPtr = nullptr;
  if (!m_bkgInputKey.key().empty()) {
    SG::ReadHandle<CscSimDataCollection> bkgContainer(m_bkgInputKey);
    if (!bkgContainer.isValid()) {   
      ATH_MSG_ERROR("Could not get background CscSimDataCollection container " << bkgContainer.name() << " from store " << bkgContainer.store());
      return StatusCode::FAILURE;
    }
    bkgContainerPtr = bkgContainer.cptr();
    
    ATH_MSG_DEBUG("Found background CscSimDataCollection container " << bkgContainer.name() << " in store " << bkgContainer.store());
  }

  SG::ReadHandle<CscSimDataCollection> signalContainer(m_signalInputKey);
  if (!signalContainer.isValid()) {
    ATH_MSG_ERROR("Could not get signal CscSimDataCollection container " << signalContainer.name() << " from store " << signalContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found signal CscSimDataCollection container " << signalContainer.name() << " in store " << signalContainer.store());

  // Creating output RDO container
  SG::WriteHandle<CscSimDataCollection> outputContainer(m_outputKey);
  ATH_CHECK(outputContainer.record(std::make_unique<CscSimDataCollection>()));
  if (!outputContainer.isValid()) {
    ATH_MSG_ERROR("Could not record output CscSimDataCollection container " << outputContainer.name() << " to store " << outputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output CscSimDataCollection container " << outputContainer.name() << " in store " << outputContainer.store());


  // Copy over signal SDO
  ATH_MSG_VERBOSE("Inserting signal SDO, size = " << signalContainer->size());
  outputContainer->insert(signalContainer->begin(), signalContainer->end());

  // Merge with background
  if (!m_bkgInputKey.key().empty()) {
    ATH_MSG_VERBOSE("Inserting background SDO, size = " << bkgContainerPtr->size());

    for (const auto &entry : *bkgContainerPtr) {
      auto it = outputContainer->find(entry.first);
      if (it != outputContainer->end()) {
        // merging needed
        const CscSimData &outSimData = it->second;
        const std::vector<CscSimData::Deposit> &depositsBkg = entry.second.getdeposits();
        std::vector<CscSimData::Deposit> depositsVector(outSimData.getdeposits());
        depositsVector.insert(depositsVector.end(), depositsBkg.begin(), depositsBkg.end());
        // fix charge
        if (!depositsVector.empty() && !depositsBkg.empty()) {
          depositsVector[0].second.setCharge(depositsVector[0].second.charge() + depositsBkg[0].second.charge());
          depositsVector[outSimData.getdeposits().size()].second.setCharge(0); // clear the first bkg deposit charge
        }
        // set the new deposits vector
        it->second = CscSimData(depositsVector);
      } else {
        outputContainer->insert(entry);
      }
    }
  }

  ATH_MSG_VERBOSE("Output SDO size " << outputContainer->size());

  ATH_MSG_DEBUG("execute() end");
  return StatusCode::SUCCESS;
}
