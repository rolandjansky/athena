/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko <agaponenko@lbl.gov>, 2006, 2007

// Ketevi A. Assamagan <ketevi@bnl.gov>, March 2008

// Piyali Banerjee <Piyali.Banerjee@cern.ch>, March 2011

#include "MdtOverlay/MdtOverlay.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonDigitContainer/MdtDigitContainer.h"

#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"

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
  MuonOverlayBase(name, pSvcLocator),
  m_storeGateTemp("StoreGateSvc/BkgEvent_1_SG", name),
  m_storeGateTempBkg("StoreGateSvc/BkgEvent_2_SG", name),
  m_mdtHelper(0),
  m_digTool("MdtDigitizationTool", this ),
  m_rdoTool("MuonRdoToMuonDigitTool", this )
{
  
  /** modifiable properties in job options */
  declareProperty("TempStore", m_storeGateTemp, "help");
  declareProperty("TempBkgStore", m_storeGateTempBkg, "help");
  declareProperty("mainInputMDT_Name", m_mainInputMDT_Name="MDT_DIGITS");
  declareProperty("overlayInputMDT_Name", m_overlayInputMDT_Name="MDT_DIGITS");
  declareProperty("IntegrationWindow", m_adcIntegrationWindow = 20.0 ); // in ns 
  declareProperty("DigitizationTool", m_digTool);
  declareProperty("ConvertRDOToDigitTool", m_rdoTool);
  declareProperty("CopyObject", m_copyObjects=false);
  declareProperty("CleanOverlayData", m_clean_overlay_data=false);//clean out the overlay data before doing overlay, so you only get MC hits in the output overlay
  declareProperty("CleanOverlaySignal", m_clean_overlay_signal=false);//clean out the signal MC before doing overlay
}

//================================================================
StatusCode MdtOverlay::overlayInitialize()
{
  msg(MSG::INFO) << "MdtOverlay initialized" << endmsg;

 if (m_storeGateTemp.retrieve().isFailure()) {
     ATH_MSG_FATAL("MdtOverlay::initialize(): TempStore for signal not found !");
     return StatusCode::FAILURE;
  }

  if (m_storeGateTempBkg.retrieve().isFailure()) {
     ATH_MSG_FATAL("MdtOverlay::initialize(): TempStoreBkg not found !");
     return StatusCode::FAILURE;
     }

  /** initialize the detectore store service */
  StoreGateSvc* detStore=0;
  StatusCode sc = serviceLocator()->service("DetectorStore", detStore);
  if (sc.isFailure()) {
    msg( MSG::FATAL ) << "DetectorStore service not found !" << endmsg;
    return StatusCode::FAILURE;
  }

  /** access to the CSC Identifier helper */
  sc = detStore->retrieve(m_mdtHelper, "MDTIDHELPER");
  if (sc.isFailure()) {
    msg( MSG::FATAL ) << "Could not get MdtIdHelper !" << endmsg;
    return StatusCode::FAILURE;
  } 
  else {
    msg( MSG::DEBUG ) << " Found the MdtIdHelper. " << endmsg;
  }

  if (m_digTool.retrieve().isFailure()) {
    msg( MSG::FATAL ) << "Could not retrieve MDT Digitization Tool!"
                      << endmsg;
    return StatusCode::FAILURE;
  }
  msg( MSG::DEBUG ) << "Retrieved MDT Digitization Tool." << endmsg;
  
  if (m_rdoTool.retrieve().isFailure()) {
    msg( MSG::FATAL ) << "Could not retrieve MDT RDO -> Digit Tool!"
                      << endmsg;
    return StatusCode::FAILURE;
  }
  msg( MSG::DEBUG ) << "Retrieved MDT RDO -> Digit Tool." << endmsg;

  return StatusCode::SUCCESS;
}

//================================================================
StatusCode MdtOverlay::overlayFinalize() 
{
  msg( MSG::INFO ) << "MdtOverlay finalized" << endmsg;
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode MdtOverlay::overlayExecute() {
  msg( MSG::DEBUG ) << "MdtOverlay::execute() begin"<< endmsg;

  //----------------------------------------------------------------
  msg( MSG::VERBOSE ) << "Retrieving data input MDT container" << endmsg;

  /** In the real data stream, run RDO -> Digit converter to make Digit
      this will be used in the overlay job */
  if ( m_rdoTool->digitize().isFailure() ) {
     msg( MSG::ERROR ) << "On the fly MDT RDO -> Digit failed " << endmsg;
     return StatusCode::FAILURE;
  }

  /** in the simulation stream, run digitization of the fly
      and make Digit - this will be used as input to the overlay job */
  if ( m_digTool->digitize().isFailure() ) {
     msg( MSG::ERROR ) << "On the fly MDT digitization failed " << endmsg;
     return StatusCode::FAILURE;
  }

  /** save a copy of the MDT Digit Container in a temp store */
  if ( m_copyObjects ) this->copyMuonIDCobject<MdtDigitContainer,MdtDigit>(&*m_storeGateMC,&*m_storeGateTemp);

    SG::ReadHandle<MdtDigitContainer> dataContainer(m_mainInputMDT_Name, m_storeGateData->name());
   if (!dataContainer.isValid()) {
     msg( MSG::ERROR ) << "Could not get data MDT container " << m_mainInputMDT_Name << endmsg;
     return StatusCode::FAILURE;
     }
   ATH_MSG_INFO("MDT Data   = "<<shortPrint(dataContainer.cptr()));

   msg( MSG::VERBOSE ) << "Retrieving MC  input MDT container" << endmsg;
   SG::ReadHandle<MdtDigitContainer> mcContainer(m_overlayInputMDT_Name,  m_storeGateMC->name());
   if(!mcContainer.isValid() ) {
     msg( MSG::ERROR ) << "Could not get overlay MDT container " << m_overlayInputMDT_Name << endmsg;
     return StatusCode::FAILURE;
   }
   ATH_MSG_INFO("MDT MC   = "<<shortPrint(mcContainer.cptr()));
   /*
   MdtDigitContainer *mdt_temp_bkg = copyMuonDigitContainer<MdtDigitContainer,MdtDigit>(dataContainer.cptr());

  if ( m_storeGateTempBkg->record(mdt_temp_bkg, m_mainInputMDT_Name).isFailure() ) {
     msg( MSG::WARNING ) << "Failed to record background MdtDigitContainer to temporary background store " << endmsg;
     }
   */
   msg( MSG::VERBOSE ) << "MDT data has digit_size "<<dataContainer->digit_size()<<endmsg;

  msg( MSG::VERBOSE ) << "MDT signal data has digit_size "<<mcContainer->digit_size()<<endmsg;
 
  SG::WriteHandle<MdtDigitContainer> outputContainer(m_mainInputMDT_Name, m_storeGateOutput->name());
  outputContainer = std::make_unique<MdtDigitContainer>(dataContainer->size());

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
  msg( MSG::DEBUG ) << "MdtOverlay::execute() end"<< endmsg;

  return StatusCode::SUCCESS;
}

//================================================================
