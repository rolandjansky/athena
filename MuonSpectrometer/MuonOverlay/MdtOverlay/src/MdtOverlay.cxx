/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko <agaponenko@lbl.gov>, 2006, 2007

// Ketevi A. Assamagan <ketevi@bnl.gov>, March 2008

// Piyali Banerjee <Piyali.Banerjee@cern.ch>, March 2011

#include "MdtOverlay/MdtOverlay.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "CxxUtils/make_unique.h"

#include "GeneratorObjects/McEventCollection.h"
#include "MuonSimData/MuonSimDataCollection.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonDigitContainer/MdtDigitContainer.h"

#include <iostream>
#include <typeinfo>
#include <stdexcept>

//================================================================
namespace Overlay {
  /** template specialization function to add 2 MDT Digits - basically the operator+=
   * A declaration of the specialization must happen before the template is used from
   * the overlayContainer() method.  So we just put this implementation at the beginning
   * of this file.
   */
  template<> void mergeChannelData(MdtDigit& mainDigit, const MdtDigit& ovlDigit, IDC_OverlayBase *baseParent) {
    MdtOverlay* parent = dynamic_cast<MdtOverlay*>(baseParent);
    if(!parent) {
      throw std::runtime_error("Overlay::mergeChannelData<MdtDigit>(): ERROR: calling alg is not MdtOverlay.");
    }

    // ----------------------------------------------------------------
    // confirm that the specialization is being used by printing out...
    static bool first_time = true;
    if(first_time) {
      first_time = false;
      parent->msg(MSG::INFO)<<"Overlay::mergeChannelData<MdtDigit>(): "
                            <<"MDT specific code is called for "
                            <<typeid(MdtDigit).name()
                            <<endmsg;
    }

    // ----------------------------------------------------------------
    // the real merging happens here
    int main_tdc = mainDigit.tdc();
    int ovl_tdc  = ovlDigit.tdc();

    /** background masks Physics hit - no correction to the ADC
        FIXME: Probably should return the maksed hit as well */
    if ( abs(main_tdc-ovl_tdc) > parent->adcIntegrationWindow() && main_tdc < ovl_tdc ) {
      // do nothing - keep mainDigit.
    }
    /** Physics hit masks the background hit - no correct to the AOD
        FIXME: Probably should return the masked hit as well */
    else if ( abs(main_tdc-ovl_tdc) > parent->adcIntegrationWindow() && main_tdc > ovl_tdc ) {
      // Use ovlDigit as the final answer
      mainDigit = ovlDigit;
    }
    /** the 2 hits overlap withing the ADC integration window
        the ADC will add partially
        the TDC is from the first hit that crosses the threshold
        FIXME: how to add partially for correct - for now just add the ADD total */
    else if ( abs(main_tdc-ovl_tdc) < parent->adcIntegrationWindow() )  {
      int tdc = std::min( mainDigit.tdc(), ovlDigit.tdc() );
      int adc = mainDigit.adc() + ovlDigit.adc();
      mainDigit =  MdtDigit(mainDigit.identify(), tdc, adc );
    }
  }

}

//================================================================
MdtOverlay::MdtOverlay(const std::string &name, ISvcLocator *pSvcLocator) :
  MuonOverlayBase(name, pSvcLocator)
{

  /** modifiable properties in job options */
  declareProperty("IntegrationWindow", m_adcIntegrationWindow); // in ns
  declareProperty("CopySDO", m_copySDO);
  declareProperty("MDTSDO", m_sdo);
  declareProperty("CleanOverlayData", m_clean_overlay_data);//clean out the overlay data before doing overlay, so you only get MC hits in the output overlay
  declareProperty("CleanOverlaySignal", m_clean_overlay_signal);//clean out the signal MC before doing overlay
}

//================================================================
StatusCode MdtOverlay::overlayInitialize()
{
  ATH_MSG_INFO("MdtOverlay initialized");

  ATH_CHECK(detStore()->retrieve(m_mdtHelper, "MDTIDHELPER"));
  ATH_MSG_DEBUG(" Found the MdtIdHelper. ");

  ATH_CHECK(m_mainInputDigitKey.initialize());
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_mainInputDigitKey );
  ATH_CHECK(m_overlayInputDigitKey.initialize());
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_overlayInputDigitKey );
  ATH_CHECK(m_outputDigitKey.initialize());
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputDigitKey );

  return StatusCode::SUCCESS;
}

//================================================================
StatusCode MdtOverlay::overlayFinalize()
{
  ATH_MSG_INFO("MdtOverlay finalized");
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode MdtOverlay::overlayExecute() {
  ATH_MSG_DEBUG("MdtOverlay::execute() begin");

  //----------------------------------------------------------------
  ATH_MSG_VERBOSE("Retrieving data input MDT container");

  SG::ReadHandle<MdtDigitContainer> dataContainer (m_mainInputDigitKey);
  if (!dataContainer.isValid()) {
    ATH_MSG_ERROR("Could not get data MDT container " << dataContainer.name() << " from store " << dataContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found data MdtDigitContainer called " << dataContainer.name() << " in store " << dataContainer.store());
  ATH_MSG_INFO("MDT Data     = "<<shortPrint(dataContainer.cptr()));

  ATH_MSG_VERBOSE("Retrieving MC  input MDT container");
  SG::ReadHandle<MdtDigitContainer> mcContainer(m_overlayInputDigitKey);
  if(!mcContainer.isValid() ) {
    ATH_MSG_ERROR("Could not get overlay MDT container " << mcContainer.name() << " from store " << mcContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found overlay MdtDigitContainer called " << mcContainer.name() << " in store " << mcContainer.store());
  ATH_MSG_INFO("MDT MC       = "<<shortPrint(mcContainer.cptr()));

  ATH_MSG_VERBOSE("MDT data has digit_size "<<dataContainer->digit_size());

  ATH_MSG_VERBOSE("MDT signal data has digit_size "<<mcContainer->digit_size());

  SG::WriteHandle<MdtDigitContainer> outputContainer(m_outputDigitKey);
  ATH_CHECK(outputContainer.record(std::make_unique<MdtDigitContainer>(dataContainer->size())));
  ATH_MSG_DEBUG("Recorded output MdtDigitContainer called " << outputContainer.name() << " in store " << outputContainer.store());

  if(dataContainer.isValid() && mcContainer.isValid() && outputContainer.isValid()) {
    if(!m_clean_overlay_data && !m_clean_overlay_signal){
      //Do the actual overlay
      this->overlayContainer(dataContainer.cptr(), mcContainer.cptr(), outputContainer.ptr());
    }
    else if (m_clean_overlay_data) {
      MdtDigitContainer nobkg(0);
      this->overlayContainer(&nobkg , mcContainer.cptr() , outputContainer.ptr());
    }
    else if (m_clean_overlay_signal) {
      MdtDigitContainer nomc(0);
      this->overlayContainer(dataContainer.cptr(), &nomc , outputContainer.ptr());
    }
  }
  ATH_MSG_INFO("MDT Result   = "<<shortPrint(outputContainer.cptr()));

  //----------------------------------------------------------------
  ATH_MSG_DEBUG("Processing MC truth data");

  // Main stream is normally real data without any MC info.
  // In tests we may use a MC generated file instead of real data.
  // Remove truth info from the main input stream, if any.
  //
  // Here we handle just MDT-specific truth classes.
  // (McEventCollection is done by the base.)

  // Now copy MDT-specific MC truth objects to the output.
  // Copy only if it is not already copied by one of other muon algorithms
  if ( m_copySDO )
    //this->copyAllObjectsOfType<MuonSimDataCollection>(&*m_storeGateOutput, &*m_storeGateMC);
    this->copyMuonObjects<MuonSimDataCollection>(&*m_storeGateOutput, &*m_storeGateMC, m_sdo);

  //----------------------------------------------------------------
  ATH_MSG_DEBUG("MdtOverlay::execute() end");

  return StatusCode::SUCCESS;
}

//================================================================
