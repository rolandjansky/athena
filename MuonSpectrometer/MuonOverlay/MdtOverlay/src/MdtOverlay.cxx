/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko <agaponenko@lbl.gov>, 2006, 2007

// Ketevi A. Assamagan <ketevi@bnl.gov>, March 2008

// Piyali Banerjee <Piyali.Banerjee@cern.ch>, March 2011

#include "MdtOverlay/MdtOverlay.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "GeneratorObjects/McEventCollection.h"
#include "MuonSimData/MuonSimDataCollection.h"

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
			    <<endreq;
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
  m_digTool("MdtDigitizationTool", this ),
  m_rdoTool("MuonRdoToMuonDigitTool", this )
{
  
  /** modifiable properties in job options */
  declareProperty("TempStore", m_storeGateTemp, "help");
  declareProperty("TempBkgStore", m_storeGateTempBkg, "help");
  declareProperty("mainInputMDT_Name", m_mainInputMDT_Name="MDT_DIGITS");
  declareProperty("overlayInputMDT_Name", m_overlayInputMDT_Name="MDT_DIGITS");
  declareProperty("IntegrationWindow", m_adcIntegrationWindow = 20.0 ); // in ns 
  declareProperty("CopySDO", m_copySDO=true);
  declareProperty("DigitizationTool", m_digTool);
  declareProperty("ConvertRDOToDigitTool", m_rdoTool);
  declareProperty("MDTSDO", m_sdo="MDT_SDO");
  declareProperty("CopyObject", m_copyObjects=false);

}

//================================================================
StatusCode MdtOverlay::overlayInitialize()
{
  msg(MSG::INFO) << "MdtOverlay initialized" << endreq;

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
    msg( MSG::FATAL ) << "DetectorStore service not found !" << endreq;
    return StatusCode::FAILURE;
  }

  /** access to the CSC Identifier helper */
  sc = detStore->retrieve(m_mdtHelper, "MDTIDHELPER");
  if (sc.isFailure()) {
    msg( MSG::FATAL ) << "Could not get MdtIdHelper !" << endreq;
    return StatusCode::FAILURE;
  } 
  else {
    msg( MSG::DEBUG ) << " Found the MdtIdHelper. " << endreq;
  }

  if (m_digTool.retrieve().isFailure()) {
    msg( MSG::FATAL ) << "Could not retrieve MDT Digitization Tool!"
                      << endreq;
    return StatusCode::FAILURE;
  }
  msg( MSG::DEBUG ) << "Retrieved MDT Digitization Tool." << endreq;
  
  if (m_rdoTool.retrieve().isFailure()) {
    msg( MSG::FATAL ) << "Could not retrieve MDT RDO -> Digit Tool!"
                      << endreq;
    return StatusCode::FAILURE;
  }
  msg( MSG::DEBUG ) << "Retrieved MDT RDO -> Digit Tool." << endreq;

  return StatusCode::SUCCESS;
}

//================================================================
StatusCode MdtOverlay::overlayFinalize() 
{
  msg( MSG::INFO ) << "MdtOverlay finalized" << endreq;
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode MdtOverlay::overlayExecute() {
  msg( MSG::DEBUG ) << "MdtOverlay::execute() begin"<< endreq;

  //----------------------------------------------------------------
  msg( MSG::VERBOSE ) << "Retrieving data input MDT container" << endreq;

  /** In the real data stream, run RDO -> Digit converter to make Digit
      this will be used in the overlay job */
  if ( m_rdoTool->digitize().isFailure() ) {
     msg( MSG::ERROR ) << "On the fly MDT RDO -> Digit failed " << endreq;
     return StatusCode::FAILURE;
  }

  /** in the simulation stream, run digitization of the fly
      and make Digit - this will be used as input to the overlay job */
  if ( m_digTool->digitize().isFailure() ) {
     msg( MSG::ERROR ) << "On the fly MDT digitization failed " << endreq;
     return StatusCode::FAILURE;
  }

  /** save a copy of the MDT Digit Container in a temp store */
  if ( m_copyObjects ) 
     this->copyMuonIDCobject<MdtDigitContainer,MdtDigit>(&*m_storeGateMC,&*m_storeGateTemp);

  std::auto_ptr<MdtDigitContainer> mdt(m_storeGateData->retrievePrivateCopy<MdtDigitContainer>(m_mainInputMDT_Name));
  if ( !mdt.get() ) {
     msg( MSG::ERROR ) << "Could not get data MDT container " << m_mainInputMDT_Name << endreq;
     return StatusCode::FAILURE;
  }
  //MdtDigitContainer * cdata = const_cast<MdtDigitContainer*>(mdt);

  msg( MSG::VERBOSE ) << "Retrieving MC  input MDT container" << endreq;
  std::auto_ptr<MdtDigitContainer> ovl_input_MDT(m_storeGateMC->retrievePrivateCopy<MdtDigitContainer>(m_overlayInputMDT_Name));
  if(!ovl_input_MDT.get() ) {
    msg( MSG::ERROR ) << "Could not get overlay MDT container " << m_overlayInputMDT_Name << endreq;
    return StatusCode::FAILURE;
  }

  MdtDigitContainer *mdt_temp_bkg = copyMuonDigitContainer<MdtDigitContainer,MdtDigit>(mdt.get());

  if ( m_storeGateTempBkg->record(mdt_temp_bkg, m_mainInputMDT_Name).isFailure() ) {
     msg( MSG::WARNING ) << "Failed to record background MdtDigitContainer to temporary background store " << endreq;
  }

  this->overlayContainer(mdt, ovl_input_MDT);

  if ( m_storeGateOutput->record(mdt, m_mainInputMDT_Name).isFailure() ) {
     msg( MSG::WARNING ) << "Failed to record MDT overlay container to output store " << endreq;
  }

  //----------------
  // This kludge is a work around for problems created by another kludge:
  // Digitization algs keep a pointer to their output Identifiable Container and reuse
  // the same object over and other again.   So unlike any "normal" per-event object
  // this IDC is not a disposable one, and we should not delete it.
  ovl_input_MDT.release();
  mdt.release();

  //----------------------------------------------------------------
  msg( MSG::DEBUG ) <<"Processing MC truth data"<<endreq;

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
  msg( MSG::DEBUG ) << "MdtOverlay::execute() end"<< endreq;

  return StatusCode::SUCCESS;
}

//================================================================
