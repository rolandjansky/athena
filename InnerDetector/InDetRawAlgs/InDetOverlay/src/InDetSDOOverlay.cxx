/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetOverlay/InDetSDOOverlay.h"

#include "InDetSimData/InDetSimData.h"

InDetSDOOverlay::InDetSDOOverlay(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name, pSvcLocator) { }

StatusCode InDetSDOOverlay::initialize()
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
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_bkgInputKey);
  ATH_CHECK( m_signalInputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_signalInputKey);
  ATH_CHECK( m_outputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputKey);

  return StatusCode::SUCCESS;
}

StatusCode InDetSDOOverlay::execute()
{
  ATH_MSG_DEBUG("execute() begin");
  
  // Reading the input containers
  ATH_MSG_VERBOSE("Retrieving input containers");

  const InDetSimDataCollection *bkgContainerPtr = nullptr;
  if (!m_bkgInputKey.key().empty()) {
    SG::ReadHandle<InDetSimDataCollection> bkgContainer(m_bkgInputKey);
    if (!bkgContainer.isValid()) {
      ATH_MSG_ERROR("Could not get background InDetSimDataCollection container " << bkgContainer.name() << " from store " << bkgContainer.store());
      return StatusCode::FAILURE;
    }
    bkgContainerPtr = bkgContainer.cptr();

    ATH_MSG_DEBUG("Found background InDetSimDataCollection container " << bkgContainer.name() << " in store " << bkgContainer.store());
  }

  SG::ReadHandle<InDetSimDataCollection> signalContainer(m_signalInputKey);
  if (!signalContainer.isValid()) {
    ATH_MSG_ERROR("Could not get signal InDetSimDataCollection container " << signalContainer.name() << " from store " << signalContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found signal InDetSimDataCollection container " << signalContainer.name() << " in store " << signalContainer.store());

  // Creating output RDO container
  SG::WriteHandle<InDetSimDataCollection> outputContainer(m_outputKey);
  ATH_CHECK(outputContainer.record(std::make_unique<InDetSimDataCollection>()));
  if (!outputContainer.isValid()) {
    ATH_MSG_ERROR("Could not record output InDetSimDataCollection container " << outputContainer.name() << " to store " << outputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output InDetSimDataCollection container " << outputContainer.name() << " in store " << outputContainer.store());

  // Copy over signal SDO
  ATH_MSG_VERBOSE("Inserting signal SDO, size = " << signalContainer->size());
  outputContainer->insert(signalContainer->begin(), signalContainer->end());

  // Merge with background
  if (!m_bkgInputKey.key().empty()) {
    ATH_MSG_VERBOSE("Inserting background SDO, size = " << bkgContainerPtr->size());

    for (const auto &entry : *bkgContainerPtr) {
      auto it = outputContainer->find(entry.first);
      if (it != outputContainer->end()) {
        // merge necessary
        const InDetSimData &outSimData = it->second;
        std::vector<InDetSimData::Deposit> depositsVector(outSimData.getdeposits());
        depositsVector.insert(depositsVector.end(), entry.second.getdeposits().begin(), entry.second.getdeposits().end());

        it->second = InDetSimData(depositsVector);
      } else {
        outputContainer->insert(entry);
      }
    }
  }

  ATH_MSG_VERBOSE("Output SDO size " << outputContainer->size());

  ATH_MSG_DEBUG("execute() end");
  return StatusCode::SUCCESS;
}
