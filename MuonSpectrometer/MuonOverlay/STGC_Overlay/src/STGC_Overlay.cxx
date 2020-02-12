/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko <agaponenko@lbl.gov>, 2006, 2007
// Ketevi A. Assamagan <ketevi@bnl.gov>, March 2008
// Piyali Banerjee <Piyali.Banerjee@cern.ch>, March 2011
// alexandre.laurier@cern.ch 2019-08-23

#include "STGC_Overlay/STGC_Overlay.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"

#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"

#include <iostream>
#include <typeinfo>
#include <stdexcept>

//================================================================
namespace Overlay {
  /** template specialization function to add 2 sTGC Digits - basically the operator+= 
   * A declaration of the specialization must happen before the template is used from
   * the overlayContainer() method.  So we just put this implementation at the beginning
   * of this file.
   */
  template<> void mergeChannelData(sTgcDigit& mainDigit, const sTgcDigit& ovlDigit, IDC_OverlayBase *baseParent) {
    STGC_Overlay* parent = dynamic_cast<STGC_Overlay*>(baseParent);
     if(!parent) {
      throw std::runtime_error("Overlay::mergeChannelData<sTgcDigit>(): ERROR: calling alg is not STGC_Overlay.");
    }
    
    // ----------------------------------------------------------------
    // confirm that the specialization is being used by printing out...
    static bool first_time = true;
    if(first_time) {
      first_time = false;
      parent->msg(MSG::INFO)<<"Overlay::mergeChannelData<sTgcDigit>(): "
			    <<"sTGC specific code is called for "
			    <<typeid(sTgcDigit).name()
			    <<endmsg;
    }
    
    // ----------------------------------------------------------------
    // the real merging happens here
    int sig_time = mainDigit.time();
    int bkg_time  = ovlDigit.time();

    /** background masks Physics hit - no correction to the ADC 
        FIXME: Probably should return the maksed hit as well */
    if ( abs(sig_time - bkg_time) > parent->timeIntegrationWindow() && sig_time < bkg_time ) {
      // do nothing - keep mainDigit.
    } 
    /** Physics hit masks the background hit - no correct to the AOD 
        FIXME: Probably should return the masked hit as well */ 
    else if ( abs(sig_time - bkg_time) > parent->timeIntegrationWindow() && sig_time > bkg_time ) {
      // Use ovlDigit as the final answer
      mainDigit = ovlDigit;
    }
    /** the 2 hits overlap within the time pulse shaping window
        the charges will add linearly
        the time is from the first hit that crosses the threshold */ 
    else if ( abs(sig_time - bkg_time) < parent->timeIntegrationWindow() )  {
      int time = std::min( mainDigit.time(), ovlDigit.time() );
      int charge = mainDigit.charge() + ovlDigit.charge();
      if (sig_time < bkg_time) // use signal digit properties
        mainDigit =  sTgcDigit(mainDigit.identify(), mainDigit.bcTag(), time, charge, mainDigit.isDead(), mainDigit.isPileup() );
      else // bkg digit came first so use its properties
        mainDigit =  sTgcDigit(ovlDigit.identify(), ovlDigit.bcTag(), time, charge, ovlDigit.isDead(), ovlDigit.isPileup() );
    }
  }

}

//================================================================
STGC_Overlay::STGC_Overlay(const std::string &name, ISvcLocator *pSvcLocator) :
  MuonOverlayBase(name, pSvcLocator),
  m_storeGateTemp("StoreGateSvc/BkgEvent_1_SG", name),
  m_storeGateTempBkg("StoreGateSvc/BkgEvent_2_SG", name),
  m_stgcHelper(0),
  m_digTool("sTgcDigitizationTool", this ),
  m_rdoTool("MuonRdoToMuonDigitTool", this )
{
  
  /** modifiable properties in job options */
  declareProperty("TempStore", m_storeGateTemp, "help");
  declareProperty("TempBkgStore", m_storeGateTempBkg, "help");
  declareProperty("mainInputSTGC_Name", m_mainInputSTGC_Name="sTGC_DIGITS");
  declareProperty("overlayInputSTGC_Name", m_overlayInputSTGC_Name="sTGC_DIGITS");
  declareProperty("IntegrationWindow", m_timeIntegrationWindow = 50.0 ); // in ns 
  declareProperty("DigitizationTool", m_digTool);
  declareProperty("ConvertRDOToDigitTool", m_rdoTool);
  declareProperty("CopyObject", m_copyObjects=false);
  declareProperty("CleanOverlayData", m_clean_overlay_data=false);//clean out the overlay data before doing overlay, so you only get MC hits in the output overlay
  declareProperty("CleanOverlaySignal", m_clean_overlay_signal=false);//clean out the signal MC before doing overlay
  declareProperty("ConvertRdoToDigit", m_convertRdo=false);// Run the RDO->Digit converter. As of 2019-08-23 this should be turned off as the input background RDO contains a Digit container already
}

//================================================================
StatusCode STGC_Overlay::overlayInitialize()
{
  msg(MSG::INFO) << "STGC_Overlay initialized" << endmsg;

 if (m_storeGateTemp.retrieve().isFailure()) {
     ATH_MSG_FATAL("STGC_Overlay::initialize(): TempStore for signal not found !");
     return StatusCode::FAILURE;
  }

  if (m_storeGateTempBkg.retrieve().isFailure()) {
     ATH_MSG_FATAL("STGC_Overlay::initialize(): TempStoreBkg not found !");
     return StatusCode::FAILURE;
     }

  /** initialize the detectore store service */
  StoreGateSvc* detStore=0;
  StatusCode sc = serviceLocator()->service("DetectorStore", detStore);
  if (sc.isFailure()) {
    msg( MSG::FATAL ) << "DetectorStore service not found !" << endmsg;
    return StatusCode::FAILURE;
  }

  /** access to the sTgc Identifier helper */
  sc = detStore->retrieve(m_stgcHelper, "STGCIDHELPER");
  if (sc.isFailure()) {
    msg( MSG::FATAL ) << "Could not get sTgcIdHelper !" << endmsg;
    return StatusCode::FAILURE;
  } 
  else {
    msg( MSG::DEBUG ) << " Found the sTgcIdHelper. " << endmsg;
  }

  if (m_digTool.retrieve().isFailure()) {
    msg( MSG::FATAL ) << "Could not retrieve sTgc Digitization Tool!"
                      << endmsg;
    return StatusCode::FAILURE;
  }
  msg( MSG::DEBUG ) << "Retrieved sTgc Digitization Tool." << endmsg;
  
  if (m_convertRdo) {
    if (m_rdoTool.retrieve().isFailure()) {
      msg( MSG::FATAL ) << "Could not retrieve sTgc RDO -> Digit Tool!"
                        << endmsg;
      return StatusCode::FAILURE;
    }
    msg( MSG::DEBUG ) << "Retrieved sTgc RDO -> Digit Tool." << endmsg;
  }

  return StatusCode::SUCCESS;
}

//================================================================
StatusCode STGC_Overlay::overlayFinalize() 
{
  msg( MSG::INFO ) << "STGC_Overlay finalized" << endmsg;
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode STGC_Overlay::overlayExecute() {
  msg( MSG::DEBUG ) << "STGC_Overlay::execute() begin"<< endmsg;

  //----------------------------------------------------------------
  msg( MSG::VERBOSE ) << "Retrieving data input STGC container" << endmsg;

  /** In the real data stream, run RDO -> Digit converter to make Digit
      Current sTGC input RDO from pileup contains a Digit container
      So no need to run RDO->Digit as it overwrites and can cause problems */
  if (m_convertRdo)
    if ( m_rdoTool->digitize().isFailure() ) {
       msg( MSG::ERROR ) << "On the fly STGC RDO -> Digit failed " << endmsg;
       return StatusCode::FAILURE;
    }

  /** in the simulation stream, run digitization of the fly
      and make Digit - this will be used as input to the overlay job */
  if ( m_digTool->digitize().isFailure() ) {
     msg( MSG::ERROR ) << "On the fly STGC digitization failed " << endmsg;
     return StatusCode::FAILURE;
  }

  /** save a copy of the STGC Digit Container in a temp store */
  if ( m_copyObjects ) this->copyMuonIDCobject<sTgcDigitContainer,sTgcDigit>(&*m_storeGateMC,&*m_storeGateTemp);

  SG::ReadHandle<sTgcDigitContainer> dataContainer(m_mainInputSTGC_Name, m_storeGateData->name());
  if (!dataContainer.isValid()) {
    msg( MSG::ERROR ) << "Could not get data STGC container " << m_mainInputSTGC_Name << endmsg;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("STGC Data   = "<<shortPrint(dataContainer.cptr()));

  msg( MSG::VERBOSE ) << "Retrieving MC  input STGC container" << endmsg;
  SG::ReadHandle<sTgcDigitContainer> mcContainer(m_overlayInputSTGC_Name,  m_storeGateMC->name());
  if(!mcContainer.isValid() ) {
    msg( MSG::ERROR ) << "Could not get overlay STGC container " << m_overlayInputSTGC_Name << endmsg;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("STGC MC   = "<<shortPrint(mcContainer.cptr()));
  msg( MSG::VERBOSE ) << "STGC data has digit_size "<<dataContainer->digit_size()<<endmsg;

  msg( MSG::VERBOSE ) << "STGC signal data has digit_size "<<mcContainer->digit_size()<<endmsg;
 
  SG::WriteHandle<sTgcDigitContainer> outputContainer(m_mainInputSTGC_Name, m_storeGateOutput->name());
  outputContainer = std::make_unique<sTgcDigitContainer>(dataContainer->size());

  if(dataContainer.isValid() && mcContainer.isValid() && outputContainer.isValid()) { 
    if(!m_clean_overlay_data && !m_clean_overlay_signal){
      //Do the actual overlay
      this->overlayContainer(dataContainer.cptr(), mcContainer.cptr(), outputContainer.ptr());
    }
    else if (m_clean_overlay_data) {
      sTgcDigitContainer nobkg(0);
      this->overlayContainer(&nobkg , mcContainer.cptr() , outputContainer.ptr());
    }
     else if (m_clean_overlay_signal) {
      sTgcDigitContainer nomc(0);
      this->overlayContainer(dataContainer.cptr(), &nomc , outputContainer.ptr());
      }
  }
  ATH_MSG_INFO("STGC Result   = "<<shortPrint(outputContainer.cptr()));

  //----------------------------------------------------------------
  msg( MSG::DEBUG ) << "STGC_Overlay::execute() end"<< endmsg;

  return StatusCode::SUCCESS;
}

//================================================================
