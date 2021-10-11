/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// AFP includes

// AFP_ByteStream2RawCnv includes
#include "AFP_ByteStream2RawCnv/AFP_RawDataProvider.h"

#include "AFP_RawEv/AFP_RawContainer.h"


AFP_RawDataProvider::AFP_RawDataProvider(const std::string &name,
                                         ISvcLocator *pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator),
      m_robDataProvider("ROBDataProviderSvc", name)
{
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

  ATH_CHECK(m_AFP_RawContainerKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode AFP_RawDataProvider::execute(const EventContext &ctx) const {
  ATH_MSG_DEBUG("AFP_RawDataProvider::EXECUTE");
  auto container = std::make_unique<AFP_RawContainer>();
  ATH_MSG_DEBUG("Created AFP RDO Container");

  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment *> listOfRobf;

  m_robDataProvider->getROBData(ctx, m_robIDs, listOfRobf);
  ATH_MSG_DEBUG(" ROB ID " << std::hex << m_robIDs<<std::dec);
  ATH_MSG_DEBUG(" Number of ROB fragments is " << listOfRobf.size());

  if (m_rawDataTool->convert(listOfRobf, container.get()).isFailure()) {
    ATH_MSG_ERROR("Bytestream conversion into raw failed");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG(" Number of time-of-flight collections in container is "
                  << container->collectionsToF().size());
    ATH_MSG_DEBUG(" Number of silicon collections in container is "
                  << container->collectionsSi().size());
  }

  SG::WriteHandle<AFP_RawContainer> writeHandle{m_AFP_RawContainerKey, ctx};
  StatusCode recordSC = writeHandle.record(std::move(container));
  if (recordSC.isFailure()) {
    ATH_MSG_WARNING("Unable to record AFP RDO Container");
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("AFP RDO Container recorded");
  }

  return StatusCode::SUCCESS;
}
