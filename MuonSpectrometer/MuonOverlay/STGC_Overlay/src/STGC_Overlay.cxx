/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko <agaponenko@lbl.gov>, 2006, 2007
// Ketevi A. Assamagan <ketevi@bnl.gov>, March 2008
// Piyali Banerjee <Piyali.Banerjee@cern.ch>, March 2011
// Alexandre Laurier <alexandre.laurier@cern.ch>, May 2019

#include <STGC_Overlay/STGC_Overlay.h>

#include <StoreGate/ReadHandle.h>
#include <StoreGate/WriteHandle.h>

#include <IDC_OverlayBase/IDC_OverlayHelpers.h>


//================================================================
namespace Overlay
{
  /** template specialization function to add 2 sTGC Digits - basically the operator+=
   * A declaration of the specialization must happen before the template is used from
   * the overlayContainer() method.  So we just put this implementation at the beginning
   * of this file.
   */
  template<>
  void mergeChannelData(sTgcDigit& signalDigit,
                        const sTgcDigit& bkgDigit,
                        const IDC_MuonOverlayBase *algorithm)
  {
    // We want to use the integration window provided by sTgcOverlay, thus the constraint
    const STGC_Overlay *parent = dynamic_cast<const STGC_Overlay *>(algorithm);
    if (!parent) {
      throw std::runtime_error("mergeChannelData<sTgcDigit>() called by a wrong parent algorithm? Must be STGC_Overlay.");
    }

    float sig_time = signalDigit.time();
    float bkg_time = bkgDigit.time();

    /** signal masks the background */
    if ( abs(sig_time - bkg_time) > parent->timeIntegrationWindow() && sig_time < bkg_time ) {
      // do nothing - keep baseDigit.
    }
    /** Background hit masks the signal hit */
    else if ( abs(sig_time - bkg_time) > parent->timeIntegrationWindow() && sig_time > bkg_time ) {
      // Use the background digit as the final answer
      signalDigit = bkgDigit;
    }
    /** the 2 hits overlap withing the time integration window
      For now, just add total charge */     
    else if ( abs(sig_time - bkg_time) < parent->timeIntegrationWindow() )  {
      // Use the earliest time
      float time = std::min( signalDigit.time(), bkgDigit.time() );
      float charge = signalDigit.charge() + bkgDigit.charge();
      //signalDigit = sTgcDigit(signalDigit.identify(), time, charge);
      signalDigit = sTgcDigit(signalDigit.identify(), signalDigit.bcTag(), time, charge, signalDigit.isDead(), signalDigit.isPileup());
    }
  }
} // namespace Overlay


//================================================================
STGC_Overlay::STGC_Overlay(const std::string &name, ISvcLocator *pSvcLocator)
  : IDC_MuonOverlayBase(name, pSvcLocator)
{
}

//================================================================
StatusCode STGC_Overlay::initialize()
{
  ATH_MSG_DEBUG("Initializing...");

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
StatusCode STGC_Overlay::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("execute() begin");


  const sTgcDigitContainer *bkgContainerPtr = nullptr;
  if (!m_bkgInputKey.empty()) {
    SG::ReadHandle<sTgcDigitContainer> bkgContainer (m_bkgInputKey, ctx);
    if (!bkgContainer.isValid()) {
      ATH_MSG_ERROR("Could not get background sTGC container " << bkgContainer.name() << " from store " << bkgContainer.store());
      return StatusCode::FAILURE;
    }
    bkgContainerPtr = bkgContainer.cptr();

    ATH_MSG_DEBUG("Found background sTGCDigitContainer called " << bkgContainer.name() << " in store " << bkgContainer.store());
    ATH_MSG_DEBUG("sTGC Background = " << Overlay::debugPrint(bkgContainer.cptr()));
    ATH_MSG_VERBOSE("sTGC background has digit_size " << bkgContainer->digit_size());
  }

  SG::ReadHandle<sTgcDigitContainer> signalContainer(m_signalInputKey, ctx);
  if(!signalContainer.isValid() ) {
    ATH_MSG_ERROR("Could not get signal sTgc container " << signalContainer.name() << " from store " << signalContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found signal sTgcDigitContainer called " << signalContainer.name() << " in store " << signalContainer.store());
  ATH_MSG_DEBUG("sTGC Signal     = " << Overlay::debugPrint(signalContainer.cptr()));
  ATH_MSG_VERBOSE("sTGC signal has digit_size " << signalContainer->digit_size());

  SG::WriteHandle<sTgcDigitContainer> outputContainer(m_outputKey, ctx);
  ATH_CHECK(outputContainer.record(std::make_unique<sTgcDigitContainer>(signalContainer->size())));
  if (!outputContainer.isValid()) {
    ATH_MSG_ERROR("Could not record output sTgcDigitContainer called " << outputContainer.name() << " to store " << outputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output sTgcDigitContainer called " << outputContainer.name() << " in store " << outputContainer.store());

  // Do the actual overlay
  ATH_CHECK(overlayContainer(bkgContainerPtr, signalContainer.cptr(), outputContainer.ptr()));
  ATH_MSG_DEBUG("sTGC Result     = " << Overlay::debugPrint(outputContainer.cptr()));

  ATH_MSG_DEBUG("execute() end");

  return StatusCode::SUCCESS;
}
