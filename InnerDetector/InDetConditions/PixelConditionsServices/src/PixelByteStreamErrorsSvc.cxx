/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelByteStreamErrorsSvc.h"
//STL includes
#include <vector>

//Gaudi includes

#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"

#include "InDetIdentifier/PixelID.h"
#include "PixelGeoModel/IBLParameterSvc.h"


#include <iostream>

using namespace std;

// Constructor
PixelByteStreamErrorsSvc::PixelByteStreamErrorsSvc( const std::string& name, 
						    ISvcLocator* pSvcLocator ) : AthService(name, pSvcLocator),
										 m_pixel_id(0),
										 m_storeGate("StoreGateSvc",name),
										 m_detStore("DetectorStore",name),
                                         m_IBLParameterSvc("IBLParameterSvc",name),
										 m_module_errors(0),
										 m_moduleROD_errors(0),
										 m_event_read(0),
										 m_FE_errors(0),
										 m_module_isread(0),
                                         m_ServiceRecords(),
                                         m_readESD(false)
{ 
  declareProperty("ReadingESD",m_readESD,"Get summary of BS errors from StoreGate, if available"); 
}

//Initialize
StatusCode PixelByteStreamErrorsSvc::initialize(){
  StatusCode sc(StatusCode::FAILURE);
  //m_log.setLevel(outputLevel());
  if (AthService::initialize() == sc) return   msg(MSG::ERROR)<<"Service failed to initialize"<<endreq, sc;

  // Get a StoreGateSvc
  if (m_storeGate.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve service " << m_storeGate << endreq;
    return StatusCode::FAILURE;
  } 
  else
    msg(MSG::INFO) << "Retrieved service " << m_storeGate << endreq;
  // Get a detector store
  if (m_detStore.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve service " << m_detStore << endreq;
    return StatusCode::FAILURE;
  }
  else
   msg(MSG::INFO) << "Retrieved service " << m_detStore << endreq ;
  
  sc = m_detStore->retrieve( m_pixel_id, "PixelID" );
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve pixel ID helper" );
    return StatusCode::FAILURE;
  }

  m_max_hashes = m_pixel_id->wafer_hash_max();

  m_module_errors = (unsigned int*) calloc(m_max_hashes, sizeof(unsigned int));
  m_moduleROD_errors = (unsigned int*) calloc(m_max_hashes, sizeof(unsigned int));
  m_event_read = (unsigned int*) calloc(m_max_hashes, sizeof(unsigned int));
  m_FE_errors = (unsigned int*) calloc(m_max_hashes, sizeof(unsigned int));
  m_module_isread = (bool*) calloc(m_max_hashes, sizeof(bool));
  // before reading first event, set all modules as active
  for (unsigned int i=0; i<m_max_hashes; i++) m_module_isread[i]=true; 

  IIncidentSvc* incsvc;
  sc = service("IncidentSvc", incsvc);
  int priority = 100;
  if( sc.isSuccess() ) {
    incsvc->addListener( this, "BeginEvent", priority);
  }




  // Get IBLParameterSvc
  if (m_IBLParameterSvc.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Could not retrieve IBLParameterSvc" << endreq;
    return StatusCode::FAILURE;
  }
  // Check if IBL is present or not
  m_ibl_is_present = m_IBLParameterSvc->containsIBL();
  m_dbm_is_present = m_IBLParameterSvc->containsDBM();


  resetCounts();
  resetPixelCounts();
  reset();
  if (sc == StatusCode::SUCCESS)
    msg(MSG::INFO) << "PixelByteStreamErrorsSvc successfully initialized" << endreq;
  return sc;
}

//Finalize
StatusCode
PixelByteStreamErrorsSvc::finalize(){
  StatusCode sc(StatusCode::SUCCESS);

  if (m_module_errors != NULL){
    free(m_module_errors);
    m_module_errors = NULL;
  }
  if (m_moduleROD_errors != NULL){
    free(m_moduleROD_errors);
    m_moduleROD_errors = NULL;
  }
  if (m_event_read != NULL && m_FE_errors!=NULL ){
    unsigned int mxevent=0;
    unsigned int mdread=0;
    unsigned int missing=0;
    unsigned int mbadfe=0;
    for (unsigned int i=0; i<m_max_hashes; i++) {
      if (m_event_read[i]==0) {
	msg(MSG::DEBUG) << "Disabled module HashId " << i << endreq;
	continue;
      }
      if (m_FE_errors[i]!=0) {
	msg(MSG::DEBUG) << "Disabled FEs module HashId " << i 
	      << " FE mask=" << std::hex << m_FE_errors[i] << std::dec << endreq;
	mbadfe++;
      }
      if (m_event_read[i]>mxevent) {
	missing += ( (m_event_read[i]-mxevent)*mdread );
	mxevent = m_event_read[i];
      } else if ( m_event_read[i]<mxevent ) {
	missing += mxevent-m_event_read[i];
      }
      mdread++;
    }
    msg(MSG::INFO) << "Found in data " << mdread 
	  << " modules out of " << m_max_hashes << endreq;
    msg(MSG::INFO) << "Found " << mxevent
	  << " events with pixel data and " << missing 
	  << " missing module fragments" << endreq; 
    msg(MSG::INFO) << "Found " 
	  << mbadfe
	  << " modules with disabled FE" << endreq;
    if (m_event_read != NULL){
      free(m_event_read);
      m_event_read = NULL;
    }
    if (m_module_isread != NULL){
      free(m_module_isread);
      m_module_isread=NULL;
    }
    if (m_FE_errors != NULL){
      free(m_FE_errors);
      m_FE_errors = NULL;
    }
    msg(MSG::INFO) << " --------------------------------------------- " << endreq;
  }

  msg(MSG::INFO) << " --- Summary from PixelByteStreamErrorsSvc --- " << endreq;
  msg(MSG::INFO) << " - Number of invalid Identifiers Errors: " << m_numInvalidIdentifiers << endreq;
  msg(MSG::INFO) << " - Number of Preamble Errors: " << m_numPreambleErrors << endreq;
  msg(MSG::INFO) << " - Number of TimeOut Errors: " << m_numTimeOutErrors << endreq;
  msg(MSG::INFO) << " - Number of LVL1ID Errors: " << m_numLVL1IDErrors << endreq;
  msg(MSG::INFO) << " - Number of BCID Errors: " << m_numBCIDErrors << endreq;
  msg(MSG::INFO) << " - Number of Flagged Errors: " << m_numFlaggedErrors << endreq;
  msg(MSG::INFO) << " - Number of Trailer Errors: " << m_numTrailerErrors << endreq;
  msg(MSG::INFO) << " - Number of Disabled FE Errors: " << m_numDisabledFEErrors << endreq;
  msg(MSG::INFO) << " - Number of ROD Errors: " << m_numRODErrors << endreq;
  msg(MSG::INFO) << " - Number of links masked by PPC: " << m_numLinkMaskedByPPC << endreq;
  msg(MSG::INFO) << " - Number of header/trailer limit errors: " << m_numLimitError << endreq;
  msg(MSG::INFO) << " - Number of Unknown word Errors: " << m_numDecodingErrors << endreq;
  msg(MSG::INFO) << " --------------------------------------------- " << endreq;
  
  
  return sc;
}


void
PixelByteStreamErrorsSvc::handle(const Incident&) {
  reset();
  if ( m_readESD && m_storeGate->contains<InDetBSErrContainer>("PixelByteStreamErrs") ) {
    StatusCode sc = readData();
    if ( sc.isFailure() ) {
      msg(MSG::ERROR) 
	    << "PixelByteStreamErrs container is registered in SG, but cannot be retrieved"
	    << endreq;
    }
  } else if ( !m_readESD ) {
    recordData();
  }
  return;
}


StatusCode 
PixelByteStreamErrorsSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IID_IPixelByteStreamErrorsSvc == riid ) {
    *ppvInterface =  dynamic_cast<IPixelByteStreamErrorsSvc*>(this);
  } else {
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}


/*
bool 
PixelByteStreamErrorsSvc::canReportAbout(InDetConditions::Hierarchy h){
  return h?(h==InDetConditions::PixelSIDE):true; 
}
*/

/*

bool 
PixelByteStreamErrorsSvc::isGood(const Identifier & elementId, InDetConditions::Hierarchy){
  const IdentifierHash elementIdHash = m_sct_id->wafer_hash(elementId);
  return isGood(elementIdHash);
}
*/

bool 
PixelByteStreamErrorsSvc::isGood(const IdentifierHash & elementIdHash) {

    Identifier dehashedId = m_pixel_id->wafer_id(elementIdHash);

    if (m_ibl_is_present || m_dbm_is_present) {
        // If module is IBL of DBM, return isActive
        if ((m_pixel_id->barrel_ec(dehashedId) == 0 && m_pixel_id->layer_disk(dehashedId) == 0)
                || m_pixel_id->is_dbm(dehashedId)) {
            return isActive(elementIdHash);
        }
    }

    int errorcode = m_module_errors[elementIdHash];
    if ((errorcode & 0xFFF1F00F) == 0) // Mask FE errors
        return isActive(elementIdHash);
    else
        return false;
}



/*
StatusCode 
PixelByteStreamErrorsSvc::fillData(){
  StatusCode sc(StatusCode::SUCCESS);
  m_filled=true;
  return sc;
}

bool
PixelByteStreamErrorsSvc::filled() const{
  return m_filled;
}
*/



void 
PixelByteStreamErrorsSvc::resetCounts() {
  m_numTimeOutErrors=0;
  m_numBCIDErrors=0;
  m_numLVL1IDErrors=0;
  m_numInvalidIdentifiers=0;
  m_numPreambleErrors=0;
  m_numTrailerErrors=0;
  m_numFlaggedErrors=0;
  m_numDisabledFEErrors=0;
  m_numDecodingErrors=0;
  m_numRODErrors=0;
  m_numLinkMaskedByPPC=0;
  m_numLimitError=0;

  // Also reset FE-I4B service records
  for (int i = 0; i < 32; ++i) m_ServiceRecords[i] = 0;

}

  
void 
PixelByteStreamErrorsSvc::resetPixelCounts() {
  for (unsigned int i=0; i<m_max_hashes; i++) {
    m_event_read[i]=0;
    m_FE_errors[i]=0;
  }
}

int 
PixelByteStreamErrorsSvc::getNumberOfErrors(int errorType) {
  switch(errorType)
    {
    case TimeOut:
      return m_numTimeOutErrors;
    case BCID:
      return m_numBCIDErrors;
    case LVL1ID:
      return m_numLVL1IDErrors;
    case Preamble:
      return m_numPreambleErrors;
    case Trailer:
      return m_numTrailerErrors;
    case Flagged:
      return m_numFlaggedErrors;
    case DisableFE:
      return m_numDisabledFEErrors;
    case ROD:
      return m_numRODErrors;
    case Decoding:
      return m_numDecodingErrors;
    case Invalid:
      return m_numInvalidIdentifiers;
    case LinkMaskedByPPC:
      return m_numLinkMaskedByPPC;
    case Limit:
      return m_numLimitError;
  }
  return 0;
}


void PixelByteStreamErrorsSvc::reset(){
  for (unsigned int i=0; i<m_max_hashes; i++) {
    m_module_errors[i]=0;
    m_moduleROD_errors[i]=0;
    m_module_isread[i]=m_readESD;
  }
}

// retrieve the data from Storegate: for one event, one entry per module with errors
StatusCode PixelByteStreamErrorsSvc::readData() {
  StatusCode sc(StatusCode::SUCCESS);  
  const InDetBSErrContainer* errCont;
  sc = m_storeGate->retrieve(errCont,"PixelByteStreamErrs");
  if (sc.isFailure() ){
    msg(MSG::ERROR) << "Failed to retrieve BS error container from SG"<<endreq;
    return sc;
  }
  std::vector<std::pair<IdentifierHash, int>* >::const_iterator it=errCont->begin();
  std::vector<std::pair<IdentifierHash, int>* >::const_iterator itEnd=errCont->end();
  for (; it != itEnd; ++it) { 
    IdentifierHash myHash=(*it)->first;
    if ( myHash<m_max_hashes )
      setModuleErrors(myHash,(*it)->second);
    else if ( myHash<2*m_max_hashes ){
      myHash-=m_max_hashes;
      m_module_isread[static_cast<unsigned int>(myHash)]=false;
    }
    else{
      myHash-=2*m_max_hashes;
      setModuleRODErrors(myHash,(*it)->second);
    }
  }
  for (unsigned int i=0; i<m_max_hashes; i++) {
    if ( m_module_isread[i] ) m_event_read[i]++;
  }
  return sc;
}

// record the data to Storegate: for one event, one entry per module with errors
StatusCode PixelByteStreamErrorsSvc::recordData() {

  InDetBSErrContainer* cont = new InDetBSErrContainer();
  StatusCode sc = m_storeGate->overwrite(cont,"PixelByteStreamErrs");
  if (sc.isFailure() ){
    msg(MSG::ERROR) <<"Failed to record/overwrite BSErrors to SG"<<endreq;
    return sc;
  }

  for (unsigned int i=0; i<m_max_hashes; i++) {
    if (m_module_errors[i] != 0){
      std::pair<IdentifierHash, int>* err = new std::pair<IdentifierHash, int>(std::make_pair((IdentifierHash)i, m_module_errors[i]));
      cont->push_back(err);
    }
    if ( !m_module_isread[i] ) {
      std::pair<IdentifierHash, int>* err = new std::pair<IdentifierHash, int>(std::make_pair((IdentifierHash)(i+m_max_hashes),1));
      cont->push_back(err);
    }
    if ( m_moduleROD_errors[i] != 0 ) {
      std::pair<IdentifierHash, int>* err = new std::pair<IdentifierHash, int>(std::make_pair((IdentifierHash)(i+2*m_max_hashes),m_moduleROD_errors[i]));
      cont->push_back(err);
    }
  }
  return sc;
}
