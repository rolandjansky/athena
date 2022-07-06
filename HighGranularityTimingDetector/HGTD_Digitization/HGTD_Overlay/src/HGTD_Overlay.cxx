/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "HGTD_Overlay.h"

#include <IDC_OverlayBase/IDC_OverlayHelpers.h>


namespace Overlay
{
  // Specialize mergeChannelData() for the HGTD
  template <>
  void mergeChannelData(HGTD_RDO &/* baseDatum */,
                        const HGTD_RDO &/* additionalDatum */,
                        const IDC_OverlayBase *algorithm)
  {
    algorithm->msg(MSG::DEBUG) << "Overlay::mergeChannelData<HGTD_RDORawData>(): "
      << "Merging of data on the same channel is not explicitly implemented for HGTD_RDORawData" << endmsg;
  }

  // Specialize copyCollection() for the HGTD
  template<>
  std::unique_ptr<HGTD_RDO_Collection> copyCollection(const IdentifierHash &hashId,
                                                      const HGTD_RDO_Collection *collection)
  {
    auto outputCollection = std::make_unique<HGTD_RDO_Collection>(hashId);
    outputCollection->setIdentifier(collection->identify());

    for (const HGTD_RDO *existingDatum : *collection) {
      // Owned by the collection
      auto *datumCopy = new HGTD_RDO(existingDatum->identify(), existingDatum->getTOA(), existingDatum->getTOT(),
                                     existingDatum->getBCID(), existingDatum->getL1ID(), existingDatum->getL1A());
      outputCollection->push_back(datumCopy);
    }

    return outputCollection;
  }
} // namespace Overlay


HGTD_Overlay::HGTD_Overlay(const std::string &name, ISvcLocator *pSvcLocator)
  : IDC_OverlayBase(name, pSvcLocator)
{
}

StatusCode HGTD_Overlay::initialize()
{
  ATH_MSG_DEBUG("Initializing...");

  // Check and initialize keys
  ATH_CHECK( m_bkgInputKey.initialize(!m_bkgInputKey.key().empty()) );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_bkgInputKey);
  ATH_CHECK( m_signalInputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_signalInputKey);
  ATH_CHECK( m_outputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputKey);

  return StatusCode::SUCCESS;
}

StatusCode HGTD_Overlay::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("execute() begin");

  // Reading the input RDOs
  ATH_MSG_VERBOSE("Retrieving input RDO containers");

  const HGTD_RDO_Container *bkgContainerPtr = nullptr;
  if (!m_bkgInputKey.empty()) {
    SG::ReadHandle<HGTD_RDO_Container> bkgContainer(m_bkgInputKey, ctx);
    if (!bkgContainer.isValid()) {
      ATH_MSG_ERROR("Could not get background HGTD RDO container " << bkgContainer.name() << " from store " << bkgContainer.store());
      return StatusCode::FAILURE;
    }
    bkgContainerPtr = bkgContainer.cptr();

    ATH_MSG_DEBUG("Found background HGTD RDO container " << bkgContainer.name() << " in store " << bkgContainer.store());
    ATH_MSG_DEBUG("HGTD Background = " << Overlay::debugPrint(bkgContainer.cptr()));
  }

  SG::ReadHandle<HGTD_RDO_Container> signalContainer(m_signalInputKey, ctx);
  if (!signalContainer.isValid()) {
    ATH_MSG_ERROR("Could not get signal HGTD RDO container " << signalContainer.name() << " from store " << signalContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found signal HGTD RDO container " << signalContainer.name() << " in store " << signalContainer.store());
  ATH_MSG_DEBUG("HGTD Signal     = " << Overlay::debugPrint(signalContainer.cptr()));

  // Creating output RDO container
  SG::WriteHandle<HGTD_RDO_Container> outputContainer(m_outputKey, ctx);
  ATH_CHECK(outputContainer.record(std::make_unique<HGTD_RDO_Container>(signalContainer->size())));
  if (!outputContainer.isValid()) {
    ATH_MSG_ERROR("Could not record output HGTD RDO container " << outputContainer.name() << " to store " << outputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output HGTD RDO container " << outputContainer.name() << " in store " << outputContainer.store());

  ATH_CHECK(overlayContainer(bkgContainerPtr, signalContainer.cptr(), outputContainer.ptr()));
  ATH_MSG_DEBUG("HGTD Result   = " << Overlay::debugPrint(outputContainer.ptr()));

  ATH_MSG_DEBUG("execute() end");
  return StatusCode::SUCCESS;
}
