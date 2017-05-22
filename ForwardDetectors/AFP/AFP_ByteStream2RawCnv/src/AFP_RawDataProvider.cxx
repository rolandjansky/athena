/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AFP includes
#include "AFP_RawEv/AFP_ROBID.h"

// AFP_ByteStream2RawCnv includes
#include "AFP_ByteStream2RawCnv/AFP_RawDataProvider.h"

AFP_RawDataProvider::AFP_RawDataProvider(const std::string &name,
                                         ISvcLocator *pSvcLocator)
    : AthAlgorithm(name, pSvcLocator), m_nRawDataCollection(0), m_nRawData(0),
      m_robDataProvider("ROBDataProviderSvc", name),
      m_rawDataTool("AFP_RawDataProviderTool"), m_AFP_RawDataCollectionKey(),
      m_collection()

{

  declareProperty("AFP_RawDataCollectionKey",
                  m_AFP_RawDataCollectionKey = "AFP_RawData");
  declareProperty("ProviderTool", m_rawDataTool);
}

AFP_RawDataProvider::~AFP_RawDataProvider() {}

StatusCode AFP_RawDataProvider::initialize() {
  ATH_MSG_DEBUG("AFP_RawDataProvider::initialize");

  if (m_robDataProvider.retrieve().isFailure()) {
    ATH_MSG_WARNING("Failed to retrieve service " << m_robDataProvider);
    return StatusCode::SUCCESS;
  } else
    ATH_MSG_DEBUG("Retrieved service " << m_robDataProvider);

  if (m_rawDataTool.retrieve().isFailure()) {
    ATH_MSG_WARNING("Failed to retrieve service " << m_rawDataTool);
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("Retrieved service " << m_rawDataTool);
  }

  return StatusCode::SUCCESS;
}

StatusCode AFP_RawDataProvider::execute() {
  ATH_MSG_DEBUG("AFP_RawDataProvider::EXECUTE");
  AFP_RawDataContainer *container = new AFP_RawDataContainer();
  ATH_MSG_DEBUG("Created AFP RDO Container");
  StatusCode recordSC =
      evtStore()->record(container, m_AFP_RawDataCollectionKey);
  if (recordSC.isFailure()) {
    ATH_MSG_WARNING("Unable to record AFP RDO Container");
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("AFP RDO Container recorded");
  }

  std::vector<const ROBFragment *> listOfRobf;
  std::vector<unsigned int> ROBIDs;
  ROBIDs.push_back(AFP_ROBID::sideA);
  ROBIDs.push_back(AFP_ROBID::sideC);

  m_robDataProvider->getROBData(ROBIDs, listOfRobf);
  ATH_MSG_DEBUG("  ROB ID " << std::hex << ROBIDs<<std::dec);
  ATH_MSG_DEBUG(" Number of ROB fragments is " << listOfRobf.size());

  if (m_rawDataTool->convert(listOfRobf, container).isFailure()) {
    ATH_MSG_WARNING("BS conversion into RDOs failed");
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG(" Number of collections in container is "
                  << container->size());
  }

  StatusCode sc = evtStore()->retrieve(container, m_AFP_RawDataCollectionKey);
  if (sc.isSuccess()) {
    ATH_MSG_DEBUG("AFP RDO Container retrieved");
  }

  return StatusCode::SUCCESS;
}
