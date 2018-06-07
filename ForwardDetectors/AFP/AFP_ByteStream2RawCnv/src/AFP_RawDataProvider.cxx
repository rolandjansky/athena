/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AFP includes

// AFP_ByteStream2RawCnv includes
#include "AFP_ByteStream2RawCnv/AFP_RawDataProvider.h"

#include "AFP_RawEv/AFP_RawContainer.h"

const std::vector<unsigned int> AFP_RawDataProvider::s_robIDs = {AFP_ROBID::sideA, AFP_ROBID::sideC};

AFP_RawDataProvider::AFP_RawDataProvider(const std::string &name,
                                         ISvcLocator *pSvcLocator)
    : AthAlgorithm(name, pSvcLocator),
      m_robDataProvider("ROBDataProviderSvc", name),
      m_rawDataTool("AFP_RawDataProviderTool")
{

  declareProperty("AFP_RawContainerKey",
                  m_AFP_RawContainerKey = "AFP_RawData",
		  "Name under which AFP_RawContainer object will be saved in StoreGate");
  
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
  AFP_RawContainer *container = new AFP_RawContainer;
  ATH_MSG_DEBUG("Created AFP RDO Container");

  StatusCode recordSC =
      evtStore()->record(container, m_AFP_RawContainerKey);
  if (recordSC.isFailure()) {
    ATH_MSG_WARNING("Unable to record AFP RDO Container");
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("AFP RDO Container recorded");
  }

  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment *> listOfRobf;

  m_robDataProvider->getROBData(s_robIDs, listOfRobf);
  ATH_MSG_DEBUG("  ROB ID " << std::hex << s_robIDs<<std::dec);
  ATH_MSG_DEBUG(" Number of ROB fragments is " << listOfRobf.size());

  if (m_rawDataTool->convert(listOfRobf, container).isFailure()) {
    ATH_MSG_WARNING("Bytestream conversion into raw failed");
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG(" Number of time-of-flight collections in container is "
                  << container->collectionsToF().size());
    ATH_MSG_DEBUG(" Number of silicon collections in container is "
                  << container->collectionsSi().size());
  }

  return StatusCode::SUCCESS;
}
