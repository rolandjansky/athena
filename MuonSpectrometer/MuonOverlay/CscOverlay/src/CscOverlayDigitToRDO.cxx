/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CscOverlay/CscOverlayDigitToRDO.h"

#include "StoreGate/ReadHandle.h"

//================================================================
CscOverlayDigitToRDO::CscOverlayDigitToRDO(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
}

//================================================================
StatusCode CscOverlayDigitToRDO::initialize()
{
  ATH_MSG_INFO("CscOverlayDigitToRDO initialized");

  ATH_CHECK(m_rdoTool2.retrieve());
  ATH_MSG_DEBUG("Retrieved CSC Digit -> RDO Tool 2.");

  ATH_CHECK(m_rdoTool4.retrieve());
  ATH_MSG_DEBUG("Retrieved CSC Digit -> RDO Tool 4.");

  ATH_CHECK( m_inputBkgRDOKey.initialize() );
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode CscOverlayDigitToRDO::execute() {
  ATH_MSG_DEBUG("CscOverlayDigitToRDO::execute() begin");
  //----------------------------------------------------------------
  unsigned int numsamples=0;//to be determined from the data
  SG::ReadHandle<CscRawDataContainer> inputBkgRDO(m_inputBkgRDOKey);
  if(!inputBkgRDO.isValid()) {
    ATH_MSG_WARNING("Could not get background CscRawDataContainer  \"" << inputBkgRDO.name() << "\" in " << inputBkgRDO.store());
    return StatusCode::SUCCESS;
  }
  ATH_MSG_VERBOSE("Found CscRawDataContainer \"" << inputBkgRDO.name() << "\" in " << inputBkgRDO.store());
  if ((inputBkgRDO->begin()==inputBkgRDO->end()) || !*(inputBkgRDO->begin())){
        ATH_MSG_WARNING("Could not get nsamples, inputBkgRDO empty?");
  }
  else{
    numsamples=inputBkgRDO->begin()->numSamples();
  }

  if (numsamples==2) {
    if ( m_rdoTool2->digitize().isFailure() ) {
      ATH_MSG_WARNING("On the fly CSC Digit -> RDO 2 failed ");
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Digitizing with 2 samples");
  }
  else if (numsamples==4) {
    if ( m_rdoTool4->digitize().isFailure() ) {
      ATH_MSG_WARNING("On the fly CSC Digit -> RDO 4 failed ");
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Digitizing with 4 samples");
  }
  else{
    ATH_MSG_WARNING("On the fly CSC Digit -> RDO failed - not 2 or 4 samples!");
    //return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("CscOverlayDigitToRDO::execute() end");
  return StatusCode::SUCCESS;
}
