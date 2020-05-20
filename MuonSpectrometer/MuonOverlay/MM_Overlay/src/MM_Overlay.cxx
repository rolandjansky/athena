/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko <agaponenko@lbl.gov>, 2006, 2007
// Ketevi A. Assamagan <ketevi@bnl.gov>, March 2008
// Piyali Banerjee <Piyali.Banerjee@cern.ch>, March 2011
// alexandre.laurier@cern.ch 2019-08-23

#include "MM_Overlay/MM_Overlay.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonDigitContainer/MmDigitContainer.h"

#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"

#include <iostream>
#include <typeinfo>
#include <stdexcept>

//================================================================
namespace Overlay {
  /** template specialization function to add 2 MM Digits - basically the operator+= 
   * A declaration of the specialization must happen before the template is used from
   * the overlayContainer() method.  So we just put this implementation at the beginning
   * of this file.
   */
  template<> void mergeChannelData(MmDigit& mainDigit, const MmDigit& ovlDigit, IDC_OverlayBase *baseParent) {
    MM_Overlay* parent = dynamic_cast<MM_Overlay*>(baseParent);
     if(!parent) {
      throw std::runtime_error("Overlay::mergeChannelData<MmDigit>(): ERROR: calling alg is not MM_Overlay.");
    }
    
    // ----------------------------------------------------------------
    // confirm that the specialization is being used by printing out...
    static bool first_time = true;
    if(first_time) {
      first_time = false;
      parent->msg(MSG::INFO)<<"Overlay::mergeChannelData<MmDigit>(): "
			    <<"MM specific code is called for "
			    <<typeid(MmDigit).name()
			    <<endmsg;
    }
    
    // ----------------------------------------------------------------
    // the real merging happens here
      
    // THIS IS PROBLEMATIC FOR MM. NEEDS TO BE ADDRESSED
    // MM digits are stored in vectors of digits
    // background and signal digits arent of same size so cant obviously compare them
    // FOR NOW, VERY TEMPORARY, ONLY USING FIRST DIGIT!!!
    // alexandre.laurier@cern.ch 
    float sig_time = mainDigit.stripResponseTime()[0]; 
    float bkg_time = ovlDigit.stripResponseTime()[0]; 


    // background masks Physics hit - no correction to the ADC 
    if ( abs(sig_time - bkg_time) > parent->timeIntegrationWindow() && sig_time < bkg_time ) {
      // do nothing - keep mainDigit.
    } 
    // Physics hit masks the background hit - no correct to the AOD 
    else if ( abs(sig_time - bkg_time) > parent->timeIntegrationWindow() && sig_time > bkg_time ) {
      // Use ovlDigit as the final answer
      mainDigit = ovlDigit;
    }
    /** the 2 hits overlap within the time pulse shaping window
        the charges will add linearly
        the time is from the first hit that crosses the threshold */ 
    else if ( abs(sig_time - bkg_time) < parent->timeIntegrationWindow() )  {
     // Use earliest time and add charge
     // Use all other values of signal digit :: FIX ME!!
      float time = std::min( sig_time, bkg_time );
      float charge = mainDigit.stripResponseCharge()[0] + ovlDigit.stripResponseCharge()[0];
      int pos = mainDigit.stripResponsePosition()[0];
      std::vector<float> Time;
      std::vector<float> Charge;
      std::vector<int> Position;
      Time.push_back(time);
      Charge.push_back(charge);
      Position.push_back(pos);
      mainDigit = MmDigit(mainDigit.identify(), Time, Position, Charge, mainDigit.chipResponseTime(), mainDigit.chipResponsePosition(), mainDigit.chipResponseCharge(), mainDigit.stripTimeForTrigger(), mainDigit.stripPositionForTrigger(), mainDigit.stripChargeForTrigger(), mainDigit.MMFE_VMM_idForTrigger(), mainDigit.VMM_idForTrigger());
    }
  }

}

//================================================================
MM_Overlay::MM_Overlay(const std::string &name, ISvcLocator *pSvcLocator) :
  MuonOverlayBase(name, pSvcLocator),
  m_storeGateTemp("StoreGateSvc/BkgEvent_1_SG", name),
  m_storeGateTempBkg("StoreGateSvc/BkgEvent_2_SG", name),
  m_mmHelper(0),
  m_digTool("MmDigitizationTool", this ),
  m_rdoTool("MuonRdoToMuonDigitTool", this )
{
  
  /** modifiable properties in job options */
  declareProperty("TempStore", m_storeGateTemp, "help");
  declareProperty("TempBkgStore", m_storeGateTempBkg, "help");
  declareProperty("mainInputMM_Name", m_mainInputMM_Name="MM_DIGITS");
  declareProperty("overlayInputMM_Name", m_overlayInputMM_Name="MM_DIGITS");
  declareProperty("IntegrationWindow", m_timeIntegrationWindow = 100.0 ); // in ns 
  declareProperty("DigitizationTool", m_digTool);
  declareProperty("ConvertRDOToDigitTool", m_rdoTool);
  declareProperty("CopyObject", m_copyObjects=false);
  declareProperty("CleanOverlayData", m_clean_overlay_data=false);//clean out the overlay data before doing overlay, so you only get MC hits in the output overlay
  declareProperty("CleanOverlaySignal", m_clean_overlay_signal=false);//clean out the signal MC before doing overlay
  declareProperty("ConvertRdoToDigit", m_convertRdo=false);// Run the RDO->Digit converter. As of 2019-08-23 this should be turned off as the input background RDO contains a Digit container already
}

//================================================================
StatusCode MM_Overlay::overlayInitialize()
{
  msg(MSG::INFO) << "MM_Overlay initialized" << endmsg;

 if (m_storeGateTemp.retrieve().isFailure()) {
     ATH_MSG_FATAL("MM_Overlay::initialize(): TempStore for signal not found !");
     return StatusCode::FAILURE;
  }

  if (m_storeGateTempBkg.retrieve().isFailure()) {
     ATH_MSG_FATAL("MM_Overlay::initialize(): TempStoreBkg not found !");
     return StatusCode::FAILURE;
     }

  /** initialize the detectore store service */
  StoreGateSvc* detStore=0;
  StatusCode sc = serviceLocator()->service("DetectorStore", detStore);
  if (sc.isFailure()) {
    msg( MSG::FATAL ) << "DetectorStore service not found !" << endmsg;
    return StatusCode::FAILURE;
  }

  /** access to the Mm Identifier helper */
  sc = detStore->retrieve(m_mmHelper, "MMIDHELPER");
  if (sc.isFailure()) {
    msg( MSG::FATAL ) << "Could not get MmIdHelper !" << endmsg;
    return StatusCode::FAILURE;
  } 
  else {
    msg( MSG::DEBUG ) << " Found the MmIdHelper. " << endmsg;
  }

  if (m_digTool.retrieve().isFailure()) {
    msg( MSG::FATAL ) << "Could not retrieve Mm Digitization Tool!"
                      << endmsg;
    return StatusCode::FAILURE;
  }
  msg( MSG::DEBUG ) << "Retrieved Mm Digitization Tool." << endmsg;
  
  if (m_convertRdo) {
    if (m_rdoTool.retrieve().isFailure()) {
      msg( MSG::FATAL ) << "Could not retrieve Mm RDO -> Digit Tool!"
                        << endmsg;
      return StatusCode::FAILURE;
    }
    msg( MSG::DEBUG ) << "Retrieved Mm RDO -> Digit Tool." << endmsg;
  }

  return StatusCode::SUCCESS;
}

//================================================================
StatusCode MM_Overlay::overlayFinalize() 
{
  msg( MSG::INFO ) << "MM_Overlay finalized" << endmsg;
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode MM_Overlay::overlayExecute() {
  msg( MSG::DEBUG ) << "MM_Overlay::execute() begin"<< endmsg;

  //----------------------------------------------------------------
  msg( MSG::VERBOSE ) << "Retrieving data input MM container" << endmsg;

  /** In the real data stream, run RDO -> Digit converter to make Digit
      Current MM input RDO from pileup contains a Digit container
      So no need to run RDO->Digit as it overwrites and can cause problems */
  if (m_convertRdo)
    if ( m_rdoTool->digitize().isFailure() ) {
       msg( MSG::ERROR ) << "On the fly MM RDO -> Digit failed " << endmsg;
       return StatusCode::FAILURE;
    }

  /** in the simulation stream, run digitization of the fly
      and make Digit - this will be used as input to the overlay job */
  if ( m_digTool->digitize().isFailure() ) {
     msg( MSG::ERROR ) << "On the fly MM digitization failed " << endmsg;
     return StatusCode::FAILURE;
  }

  /** save a copy of the MM Digit Container in a temp store */
  if ( m_copyObjects ) this->copyMuonIDCobject<MmDigitContainer,MmDigit>(&*m_storeGateMC,&*m_storeGateTemp);

  SG::ReadHandle<MmDigitContainer> dataContainer(m_mainInputMM_Name, m_storeGateData->name());
  if (!dataContainer.isValid()) {
    msg( MSG::ERROR ) << "Could not get data MM container " << m_mainInputMM_Name << endmsg;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("MM Data   = "<<shortPrint(dataContainer.cptr()));

  msg( MSG::VERBOSE ) << "Retrieving MC  input MM container" << endmsg;
  SG::ReadHandle<MmDigitContainer> mcContainer(m_overlayInputMM_Name,  m_storeGateMC->name());
  if(!mcContainer.isValid() ) {
    msg( MSG::ERROR ) << "Could not get overlay MM container " << m_overlayInputMM_Name << endmsg;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("MM MC   = "<<shortPrint(mcContainer.cptr()));
  msg( MSG::VERBOSE ) << "MM data has digit_size "<<dataContainer->digit_size()<<endmsg;

  msg( MSG::VERBOSE ) << "MM signal data has digit_size "<<mcContainer->digit_size()<<endmsg;
 
  SG::WriteHandle<MmDigitContainer> outputContainer(m_mainInputMM_Name, m_storeGateOutput->name());
  outputContainer = std::make_unique<MmDigitContainer>(dataContainer->size());

  if(dataContainer.isValid() && mcContainer.isValid() && outputContainer.isValid()) { 
    if(!m_clean_overlay_data && !m_clean_overlay_signal){
      //Do the actual overlay
      this->overlayContainer(dataContainer.cptr(), mcContainer.cptr(), outputContainer.ptr());
    }
    else if (m_clean_overlay_data) {
      MmDigitContainer nobkg(0);
      this->overlayContainer(&nobkg , mcContainer.cptr() , outputContainer.ptr());
    }
     else if (m_clean_overlay_signal) {
      MmDigitContainer nomc(0);
      this->overlayContainer(dataContainer.cptr(), &nomc , outputContainer.ptr());
      }
  }
  ATH_MSG_INFO("MM Result   = "<<shortPrint(outputContainer.cptr()));

  //----------------------------------------------------------------
  msg( MSG::DEBUG ) << "MM_Overlay::execute() end"<< endmsg;

  return StatusCode::SUCCESS;
}

//================================================================
