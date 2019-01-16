/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelByteStreamErrorsSvc.h"
//STL includes
#include <vector>

//Gaudi includes

#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"

#include "InDetIdentifier/PixelID.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include <memory>


using namespace std;

// Constructor
PixelByteStreamErrorsSvc::PixelByteStreamErrorsSvc( const std::string& name, 
    ISvcLocator* pSvcLocator ) : AthService(name, pSvcLocator),
  m_pixel_id(0),
  m_storeGate("StoreGateSvc",name),
  m_detStore("DetectorStore",name),
  m_module_errors(0),
  m_moduleROD_errors(0),
  m_event_read(0),
  m_FE_errors(0),
  m_module_isread(0),
  m_ServiceRecords(),
  m_checkError(0),
  m_max_hashes(0),
  m_readESD(false)
{ 
  declareProperty("ReadingESD",     m_readESD,"Get summary of BS errors from StoreGate, if available"); 
  declareProperty("BSErrContainer", m_BSErrContainerKey=std::string("PixelByteStreamErrs"));
  resetCounts();
}

//Initialize
StatusCode PixelByteStreamErrorsSvc::initialize() {
  CHECK(AthService::initialize());

  // Get a StoreGateSvc
  CHECK(m_storeGate.retrieve()); 

  // Get a detector store
  CHECK(m_detStore.retrieve());
  
  // Pixel ID
  CHECK(m_detStore->retrieve(m_pixel_id,"PixelID"));

  m_BSErrContReadKey=m_BSErrContainerKey;
  ATH_CHECK(m_BSErrContReadKey.initialize());

  m_BSErrContWriteKey=m_BSErrContainerKey;
  ATH_CHECK(m_BSErrContWriteKey.initialize());

  m_max_hashes = m_pixel_id->wafer_hash_max();

  m_module_errors = (unsigned int*) calloc(m_max_hashes, sizeof(unsigned int));
  m_moduleROD_errors = (unsigned int*) calloc(m_max_hashes, sizeof(unsigned int));
  m_event_read = (unsigned int*) calloc(m_max_hashes, sizeof(unsigned int));
  m_FE_errors = (unsigned int*) calloc(m_max_hashes, sizeof(unsigned int));
  m_module_isread = (bool*) calloc(m_max_hashes, sizeof(bool));
  // before reading first event, set all modules as active
  for (unsigned int i=0; i<m_max_hashes; i++) { m_module_isread[i]=true; }

  IIncidentSvc* incsvc;
  int priority = 100;
  if (service("IncidentSvc",incsvc).isSuccess()) {
    incsvc->addListener( this, "BeginEvent", priority);
  }

  resetCounts();
  resetPixelCounts();
  reset();
  return StatusCode::SUCCESS;
}

//Finalize
StatusCode PixelByteStreamErrorsSvc::finalize() {

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
        ATH_MSG_DEBUG("Disabled module HashId " << i);
        continue;
      }
      if (m_FE_errors[i]!=0) {
        ATH_MSG_DEBUG("Disabled FEs module HashId " << i << " FE mask=" << std::hex << m_FE_errors[i] << std::dec);
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
    ATH_MSG_INFO("Found in data " << mdread << " modules out of " << m_max_hashes);
    ATH_MSG_INFO("Found " << mxevent << " events with pixel data and " << missing << " missing module fragments");
    ATH_MSG_INFO("Found " << mbadfe << " modules with disabled FE");

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
    ATH_MSG_INFO(" --------------------------------------------- ");
  }

  ATH_MSG_INFO(" --- Summary from PixelByteStreamErrorsSvc --- ");
  ATH_MSG_INFO(" - Number of invalid Identifiers Errors: " << m_numInvalidIdentifiers);
  ATH_MSG_INFO(" - Number of Preamble Errors: " << m_numPreambleErrors);
  ATH_MSG_INFO(" - Number of TimeOut Errors: " << m_numTimeOutErrors);
  ATH_MSG_INFO(" - Number of LVL1ID Errors: " << m_numLVL1IDErrors);
  ATH_MSG_INFO(" - Number of BCID Errors: " << m_numBCIDErrors);
  ATH_MSG_INFO(" - Number of Flagged Errors: " << m_numFlaggedErrors);
  ATH_MSG_INFO(" - Number of Trailer Errors: " << m_numTrailerErrors);
  ATH_MSG_INFO(" - Number of Disabled FE Errors: " << m_numDisabledFEErrors);
  ATH_MSG_INFO(" - Number of ROD Errors: " << m_numRODErrors);
  ATH_MSG_INFO(" - Number of links masked by PPC: " << m_numLinkMaskedByPPC);
  ATH_MSG_INFO(" - Number of header/trailer limit errors: " << m_numLimitError);
  ATH_MSG_INFO(" - Number of Unknown word Errors: " << m_numDecodingErrors);
  ATH_MSG_INFO(" --------------------------------------------- ");

  return StatusCode::SUCCESS;
}


void PixelByteStreamErrorsSvc::handle(const Incident&) {
  reset();

  if (m_readESD) {
    if (readData().isFailure()) {
      ATH_MSG_ERROR("PixelByteStreamErrs container is registered in SG, but cannot be retrieved");
    }
  } 
  else if (!m_readESD) {
    recordData();
  }
  return;
}

StatusCode PixelByteStreamErrorsSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (IPixelByteStreamErrorsSvc::interfaceID().versionMatch(riid)) {
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

bool PixelByteStreamErrorsSvc::isGood(const IdentifierHash & elementIdHash) {
  Identifier dehashedId = m_pixel_id->wafer_id(elementIdHash);

  if (m_pixel_id->wafer_hash_max()==2048) {   // RUN-2 setup
    // If module is IBL of DBM, return isActive
    if ((m_pixel_id->barrel_ec(dehashedId) == 0 && m_pixel_id->layer_disk(dehashedId) == 0)
        || m_pixel_id->is_dbm(dehashedId)) {
      return isActive(elementIdHash);
    }
  }

  int errorcode = m_module_errors[elementIdHash];
  if ((errorcode & 0xFFF1F00F) == 0) { // Mask FE errors
    return isActive(elementIdHash);
  }
  else {
    return false;
  }
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

void PixelByteStreamErrorsSvc::resetCounts() {
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
  for (int i = 0; i < 32; ++i) { m_ServiceRecords[i]=0; }
}

  
void PixelByteStreamErrorsSvc::resetPixelCounts() {
  for (unsigned int i=0; i<m_max_hashes; i++) {
    m_event_read[i]=0;
    m_FE_errors[i]=0;
  }
  m_all_FE_errors.clear();
}

int PixelByteStreamErrorsSvc::getNumberOfErrors(int errorType) {
  switch(errorType) {
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
  m_all_FE_errors.clear();
}

// retrieve the data from Storegate: for one event, one entry per module with errors
StatusCode PixelByteStreamErrorsSvc::readData() {

  SG::ReadHandle<InDetBSErrContainer> errCont(m_BSErrContReadKey);
  if (!errCont.isValid()) {
    ATH_MSG_ERROR("Failed to retrieve BS error container from SG");
    return StatusCode::FAILURE;
  }

  for (const auto* elt : *errCont) {
    IdentifierHash myHash=elt->first;
    if ( myHash<m_max_hashes )
      setModuleErrors(myHash,elt->second);
    else if ( myHash<2*m_max_hashes ){
      myHash-=m_max_hashes;
      m_module_isread[static_cast<unsigned int>(myHash)]=false;
    }
    else{
      myHash-=2*m_max_hashes;
      setModuleRODErrors(myHash,elt->second);
    }
  }
  for (unsigned int i=0; i<m_max_hashes; i++) {
    if ( m_module_isread[i] ) m_event_read[i]++;
  }

  return StatusCode::SUCCESS;
}

// record the data to Storegate: for one event, one entry per module with errors
StatusCode PixelByteStreamErrorsSvc::recordData() {

  std::unique_ptr<InDetBSErrContainer> cont = std::make_unique<InDetBSErrContainer>();
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

  StatusCode sc = StatusCode::SUCCESS;
  if (cont->size()==m_pixel_id->wafer_hash_max()) {
    m_checkError = -1;
  }
  else if (cont->size()!=m_checkError) {
    m_checkError = cont->size();
    m_BSErrContWrite = SG::makeHandle(m_BSErrContWriteKey);
    sc = m_BSErrContWrite.record(std::move(cont));
  }

  if (sc.isFailure() ){
    ATH_MSG_ERROR("Failed to record/overwrite BSErrors to SG");
    return sc;
  }

  return sc;
}
