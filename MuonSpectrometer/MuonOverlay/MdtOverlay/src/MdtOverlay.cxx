/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko <agaponenko@lbl.gov>, 2006, 2007
// Ketevi A. Assamagan <ketevi@bnl.gov>, March 2008
// Piyali Banerjee <Piyali.Banerjee@cern.ch>, March 2011

#include <MdtOverlay/MdtOverlay.h>

#include <StoreGate/ReadHandle.h>
#include <StoreGate/WriteHandle.h>

#include <IDC_OverlayBase/IDC_OverlayHelpers.h>


//================================================================
namespace Overlay
{
  /** template specialization function to add 2 MDT Digits - basically the operator+=
   * A declaration of the specialization must happen before the template is used from
   * the overlayContainer() method.  So we just put this implementation at the beginning
   * of this file.
   */
  template<>
  void mergeChannelData(MdtDigit& signalDigit,
                        const MdtDigit& bkgDigit,
                        IDC_MuonOverlayBase *algorithm)
  {
    // We want to use the integration window provided by MdtOverlay, thus the constraint
    MdtOverlay *parent = dynamic_cast<MdtOverlay *>(algorithm);
    if (!parent) {
      throw std::runtime_error("mergeChannelData<MdtDigit>() called by a wrong parent algorithm? Must be MdtOverlay.");
    }

    int sig_tdc = signalDigit.tdc();
    int bkg_tdc = bkgDigit.tdc();

    /** signal masks the background - no correction to the ADC
        FIXME: Probably should return the masked hit as well */
    if ( abs(sig_tdc - bkg_tdc) > parent->adcIntegrationWindow() && sig_tdc < bkg_tdc ) {
      // do nothing - keep baseDigit.
    }
    /** Physics hit masks the background hit - no correct to the AOD
        FIXME: Probably should return the masked hit as well */
    else if ( abs(sig_tdc - bkg_tdc) > parent->adcIntegrationWindow() && sig_tdc > bkg_tdc ) {
      // Use the background digit as the final answer
      signalDigit = bkgDigit;
    }
    /** the 2 hits overlap withing the ADC integration window
        the ADC will add partially
        the TDC is from the first hit that crosses the threshold
        FIXME: how to add partially for correct - for now just add the ADD total */
    else if ( abs(sig_tdc - bkg_tdc) < parent->adcIntegrationWindow() )  {
      int tdc = std::min( signalDigit.tdc(), bkgDigit.tdc() );
      int adc = signalDigit.adc() + bkgDigit.adc();
      signalDigit = MdtDigit(signalDigit.identify(), tdc, adc);
    }
  }
} // namespace Overlay


//================================================================
MdtOverlay::MdtOverlay(const std::string &name, ISvcLocator *pSvcLocator)
  : IDC_MuonOverlayBase(name, pSvcLocator)
{
}

//================================================================
StatusCode MdtOverlay::initialize()
{
  ATH_MSG_DEBUG("Initializing...");

  if (!m_includeBkg) {
    ATH_MSG_DEBUG("Disabling use of background RDOs...");
    ATH_CHECK( m_bkgInputKey.assign("") );
  }

  // Check and initialize keys
  ATH_CHECK( m_bkgInputKey.initialize(!m_bkgInputKey.key().empty()) );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_bkgInputKey );
  ATH_CHECK(m_signalInputKey.initialize());
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_signalInputKey );
  ATH_CHECK(m_outputKey.initialize());
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputKey );

  return StatusCode::SUCCESS;
}

//================================================================
StatusCode MdtOverlay::execute() {
  ATH_MSG_DEBUG("execute() begin");


  const MdtDigitContainer *bkgContainerPtr = nullptr;
  if (m_includeBkg) {
    SG::ReadHandle<MdtDigitContainer> bkgContainer (m_bkgInputKey);
    if (!bkgContainer.isValid()) {
      ATH_MSG_ERROR("Could not get background MDT container " << bkgContainer.name() << " from store " << bkgContainer.store());
      return StatusCode::FAILURE;
    }
    bkgContainerPtr = bkgContainer.cptr();

    ATH_MSG_DEBUG("Found background MdtDigitContainer called " << bkgContainer.name() << " in store " << bkgContainer.store());
    ATH_MSG_DEBUG("MDT Background = " << Overlay::debugPrint(bkgContainer.cptr()));
    ATH_MSG_VERBOSE("MDT background has digit_size " << bkgContainer->digit_size());
  }

  SG::ReadHandle<MdtDigitContainer> signalContainer(m_signalInputKey);
  if(!signalContainer.isValid() ) {
    ATH_MSG_ERROR("Could not get signal MDT container " << signalContainer.name() << " from store " << signalContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found signal MdtDigitContainer called " << signalContainer.name() << " in store " << signalContainer.store());
  ATH_MSG_DEBUG("MDT Signal     = " << Overlay::debugPrint(signalContainer.cptr()));
  ATH_MSG_VERBOSE("MDT signal has digit_size " << signalContainer->digit_size());

  SG::WriteHandle<MdtDigitContainer> outputContainer(m_outputKey);
  ATH_CHECK(outputContainer.record(std::make_unique<MdtDigitContainer>(signalContainer->size())));
  if (!outputContainer.isValid()) {
    ATH_MSG_ERROR("Could not record output MdtDigitContainer called " << outputContainer.name() << " to store " << outputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output MdtDigitContainer called " << outputContainer.name() << " in store " << outputContainer.store());

  // Do the actual overlay
  ATH_CHECK(overlayContainer(bkgContainerPtr, signalContainer.cptr(), outputContainer.ptr()));
  ATH_MSG_DEBUG("MDT Result     = " << Overlay::debugPrint(outputContainer.cptr()));


  ATH_MSG_DEBUG("execute() end");

  return StatusCode::SUCCESS;
}
