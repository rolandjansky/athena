/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetOverlay/PixelOverlay.h"

#include "IDC_OverlayBase/IDC_OverlayHelpers.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

namespace Overlay
{
  // Specialize copyCollection() for the Pixel
  template<>
  void copyCollection(const InDetRawDataCollection<PixelRDORawData> *input_coll,
                      InDetRawDataCollection<PixelRDORawData> *copy_coll)
  {
    copy_coll->setIdentifier(input_coll->identify());
    InDetRawDataCollection<PixelRDORawData>::const_iterator firstData = input_coll->begin();
    InDetRawDataCollection<PixelRDORawData>::const_iterator lastData = input_coll->end();	
    for ( ; firstData != lastData; ++firstData)
    {	
      const Identifier ident = (*firstData)->identify();
      const unsigned int word = (*firstData)->getWord();
      Pixel1RawData *newData = new Pixel1RawData(ident, word);
      copy_coll->push_back(newData);
    }
  }
} // namespace Overlay


PixelOverlay::PixelOverlay(const std::string &name, ISvcLocator *pSvcLocator)
  : IDC_OverlayBase(name, pSvcLocator)
{
}

StatusCode PixelOverlay::initialize()
{
  ATH_MSG_DEBUG("Initializing...");

  if (!m_includeBkg) {
    ATH_MSG_DEBUG("Disabling use of background RDOs...");
    ATH_CHECK( m_bkgInputKey.assign("") );
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

StatusCode PixelOverlay::execute()
{
  ATH_MSG_DEBUG("execute() begin");

  // Reading the input RDOs
  ATH_MSG_VERBOSE("Retrieving input RDO containers");

  const PixelRDO_Container *bkgContainerPtr = nullptr;
  if (m_includeBkg) {
    SG::ReadHandle<PixelRDO_Container> bkgContainer(m_bkgInputKey);
    if (!bkgContainer.isValid()) {
      ATH_MSG_ERROR("Could not get background Pixel RDO container " << bkgContainer.name() << " from store " << bkgContainer.store());
      return StatusCode::FAILURE;
    }
    bkgContainerPtr = bkgContainer.cptr();

    ATH_MSG_DEBUG("Found background Pixel RDO container " << bkgContainer.name() << " in store " << bkgContainer.store());
    ATH_MSG_DEBUG("Pixel Background = " << Overlay::debugPrint(bkgContainer.cptr()));
  }

  SG::ReadHandle<PixelRDO_Container> signalContainer(m_signalInputKey);
  if (!signalContainer.isValid()) {
    ATH_MSG_ERROR("Could not get signal Pixel RDO container " << signalContainer.name() << " from store " << signalContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found signal Pixel RDO container " << signalContainer.name() << " in store " << signalContainer.store());
  ATH_MSG_DEBUG("Pixel Signal     = " << Overlay::debugPrint(signalContainer.cptr()));

  // Creating output RDO container
  SG::WriteHandle<PixelRDO_Container> outputContainer(m_outputKey);
  ATH_CHECK(outputContainer.record(std::make_unique<PixelRDO_Container>(signalContainer->size())));
  ATH_MSG_DEBUG("Recorded output Pixel RDO container " << outputContainer.name() << " in store " << outputContainer.store());

  if (outputContainer.isValid()) {
    overlayContainerNew(bkgContainerPtr, signalContainer.cptr(), outputContainer.ptr());

    ATH_MSG_DEBUG("Pixel Result   = " << Overlay::debugPrint(outputContainer.ptr()));
  }

  ATH_MSG_DEBUG("execute() end");
  return StatusCode::SUCCESS;
}
