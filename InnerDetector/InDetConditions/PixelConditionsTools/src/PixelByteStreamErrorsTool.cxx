/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelByteStreamErrorsTool.h"

PixelByteStreamErrorsTool::PixelByteStreamErrorsTool(const std::string& type, const std::string& name, const IInterface* parent)
  :AthAlgTool(type, name, parent),
  m_pixelID(nullptr),
  m_ServiceRecords(),
  m_readESD(false)
{ 
  declareProperty("ReadingESD",     m_readESD,"Get summary of BS errors from StoreGate, if available"); 
  resetCounts();
}

//Initialize
StatusCode PixelByteStreamErrorsTool::initialize() {

  ATH_CHECK(detStore()->retrieve(m_pixelID,"PixelID"));

  if (m_readESD) {
    ATH_CHECK(m_BSErrContReadKey.initialize());
    renounce(m_BSErrContWriteKey);
  }
  else {
    renounce(m_BSErrContReadKey);
    ATH_CHECK(m_BSErrContWriteKey.initialize());
  }

  m_module_errors.reserve(m_pixelID->wafer_hash_max());
  m_moduleROD_errors.reserve(m_pixelID->wafer_hash_max());
  m_event_read.reserve(m_pixelID->wafer_hash_max());
  m_FE_errors.reserve(m_pixelID->wafer_hash_max());
  m_module_isread.reserve(m_pixelID->wafer_hash_max());

  // before reading first event, set all modules as active
  for (unsigned int i=0; i<m_pixelID->wafer_hash_max(); i++) { m_module_isread[i]=true; }

  resetCounts();

  for (unsigned int i=0; i<m_pixelID->wafer_hash_max(); i++) {
    m_event_read[i] = 0;
    m_FE_errors[i]  = 0;
  }

  for (unsigned int i=0; i<m_pixelID->wafer_hash_max(); i++) {
    m_module_errors[i]=0;
    m_moduleROD_errors[i]=0;
    m_module_isread[i]=m_readESD;
  }
  m_all_FE_errors.clear();

  return StatusCode::SUCCESS;
}

//Finalize
StatusCode PixelByteStreamErrorsTool::finalize() {

  if (m_event_read.size() && m_FE_errors.size()) {
    unsigned int mxevent=0;
    unsigned int mdread=0;
    unsigned int missing=0;
    unsigned int mbadfe=0;
    for (unsigned int i=0; i<m_pixelID->wafer_hash_max(); i++) {
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
    ATH_MSG_INFO("Found in data " << mdread << " modules out of " << m_pixelID->wafer_hash_max());
    ATH_MSG_INFO("Found " << mxevent << " events with pixel data and " << missing << " missing module fragments");
    ATH_MSG_INFO("Found " << mbadfe << " modules with disabled FE");
    ATH_MSG_INFO(" --------------------------------------------- ");
  }

  ATH_MSG_INFO(" --- Summary from PixelByteStreamErrorsTool --- ");
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

bool PixelByteStreamErrorsTool::isGood(const IdentifierHash & elementIdHash) const {
  Identifier dehashedId = m_pixelID->wafer_id(elementIdHash);

  if (m_pixelID->wafer_hash_max()==2048) {   // RUN-2 setup
    // If module is IBL of DBM, return isActive
    if ((m_pixelID->barrel_ec(dehashedId) == 0 && m_pixelID->layer_disk(dehashedId) == 0)
        || m_pixelID->is_dbm(dehashedId)) {
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

unsigned int PixelByteStreamErrorsTool::getModuleErrors(IdentifierHash hashID) const {
  return m_module_errors[(int) hashID];
}

void PixelByteStreamErrorsTool::setModuleErrors(IdentifierHash hashID, unsigned int errorcode) const {
  if (static_cast<unsigned int>(hashID)<m_pixelID->wafer_hash_max()) {
    m_module_errors[static_cast<unsigned int>(hashID)] = errorcode;
  }
}

void PixelByteStreamErrorsTool::setModuleRODErrors(IdentifierHash hashID, unsigned int errorcode) const {
  if (static_cast<unsigned int>(hashID)<m_pixelID->wafer_hash_max()) {
    m_moduleROD_errors[static_cast<unsigned int>(hashID)] = errorcode;
  }
}

std::map<IdentifierHash, std::map<unsigned int, unsigned int> > & PixelByteStreamErrorsTool::getAllFeErrors() const {
  return m_all_FE_errors;
}

unsigned int PixelByteStreamErrorsTool::getFeErrorCode(IdentifierHash module, unsigned int fe_number) const {
  std::map<IdentifierHash, std::map<unsigned int, unsigned int> >::iterator mod_itr = m_all_FE_errors.find(module);
  if (mod_itr != m_all_FE_errors.end()) {
    std::map<unsigned int, unsigned int>::iterator fe_itr = mod_itr->second.find(fe_number);
    if (fe_itr != mod_itr->second.end()) {
      return fe_itr->second;
    }
  }
  return 0;
} 

void PixelByteStreamErrorsTool::setFeErrorCode(IdentifierHash module, unsigned int fe_number, unsigned int errorcode) const {
  m_all_FE_errors[module][fe_number] = errorcode;
}

void PixelByteStreamErrorsTool::setModuleFragmentSize(IdentifierHash hashID, unsigned int size) const {
  m_module_fragment_size[hashID] = size;
}

unsigned int PixelByteStreamErrorsTool::getModuleFragmentSize(IdentifierHash hashID) const {
  std::map<IdentifierHash, unsigned int>::iterator itr = m_module_fragment_size.find(hashID);
  if (itr != m_module_fragment_size.end()) {
    return itr->second;
  }
  return 0;
}

void PixelByteStreamErrorsTool::resetCounts() const {
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

int PixelByteStreamErrorsTool::getNumberOfErrors(int errorType) const {
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

void PixelByteStreamErrorsTool::addInvalidIdentifier() const {
  m_numInvalidIdentifiers++;
}

void PixelByteStreamErrorsTool::addPreambleError() const {
  m_numPreambleErrors++;
}

void PixelByteStreamErrorsTool::addTimeOutError() const {
  m_numTimeOutErrors++;
}

void PixelByteStreamErrorsTool::addLVL1IDError() const {
  m_numLVL1IDErrors++;
}

void PixelByteStreamErrorsTool::addBCIDError() const {
  m_numBCIDErrors++;
}

void PixelByteStreamErrorsTool::addFlaggedError() const {
  m_numFlaggedErrors++;
}

void PixelByteStreamErrorsTool::addTrailerError() const {
  m_numTrailerErrors++;
}

void PixelByteStreamErrorsTool::addDisabledFEError() const {
  m_numDisabledFEErrors++;
}

void PixelByteStreamErrorsTool::addDecodingError() const {
  m_numDecodingErrors++;
}

void PixelByteStreamErrorsTool::addRODError() const {
  m_numRODErrors++;
}

void PixelByteStreamErrorsTool::addRead(IdentifierHash hashID) const {
  if (static_cast<unsigned int>(hashID)<m_pixelID->wafer_hash_max()) {
    m_event_read[static_cast<unsigned int>(hashID)]++;
    m_module_isread[static_cast<unsigned int>(hashID)]=true;
  }
}

unsigned int PixelByteStreamErrorsTool::getReadEvents(IdentifierHash hashID) const {
  return m_event_read[(int)hashID];
}

bool PixelByteStreamErrorsTool::isActive(IdentifierHash hashID) const {
  return m_module_isread[(int)hashID];
}

void PixelByteStreamErrorsTool::addBadFE(IdentifierHash hashID, int badFE) const {
  if (static_cast<unsigned int>(hashID)<m_pixelID->wafer_hash_max()) {
    m_FE_errors[static_cast<unsigned int>(hashID)] |= (1<<badFE);
  }
}

unsigned int PixelByteStreamErrorsTool::getBadFE(IdentifierHash hashID) const {
  return m_FE_errors[(int)hashID];
}

StatusCode PixelByteStreamErrorsTool::readData() {

  SG::ReadHandle<InDetBSErrContainer> errCont(m_BSErrContReadKey);
  if (!errCont.isValid()) {
    ATH_MSG_ERROR("Failed to retrieve BS error container from SG");
    return StatusCode::FAILURE;
  }

  for (unsigned int i=0; i<m_pixelID->wafer_hash_max(); i++) {
    m_event_read[i] = 0;
    m_module_errors[i]=0;
    m_moduleROD_errors[i]=0;
    m_module_isread[i]=m_readESD;
  }

  for (const auto* elt : *errCont) {
    IdentifierHash myHash=elt->first;
    if (myHash<m_pixelID->wafer_hash_max()) { 
      setModuleErrors(myHash,elt->second); 
    }
    else if (myHash<2*m_pixelID->wafer_hash_max()) {
      myHash-=m_pixelID->wafer_hash_max();
      m_module_isread[static_cast<unsigned int>(myHash)]=false;
    }
    else {
      myHash-=2*m_pixelID->wafer_hash_max();
      setModuleRODErrors(myHash,elt->second);
    }
  }
  for (unsigned int i=0; i<m_pixelID->wafer_hash_max(); i++) {
    if (m_module_isread[i]) { m_event_read[i]++; }
  }

  return StatusCode::SUCCESS;
}

// record the data to Storegate: for one event, one entry per module with errors
StatusCode PixelByteStreamErrorsTool::recordData() const {

  std::unique_ptr<InDetBSErrContainer> cont = std::make_unique<InDetBSErrContainer>();
  for (unsigned int i=0; i<m_pixelID->wafer_hash_max(); i++) {
    if (m_module_errors[i] != 0){
      std::pair<IdentifierHash, int>* err = new std::pair<IdentifierHash, int>(std::make_pair((IdentifierHash)i, m_module_errors[i]));
      cont->push_back(err);
    }
    if ( !m_module_isread[i] ) {
      std::pair<IdentifierHash, int>* err = new std::pair<IdentifierHash, int>(std::make_pair((IdentifierHash)(i+m_pixelID->wafer_hash_max()),1));
      cont->push_back(err);
    }
    if ( m_moduleROD_errors[i] != 0 ) {
      std::pair<IdentifierHash, int>* err = new std::pair<IdentifierHash, int>(std::make_pair((IdentifierHash)(i+2*m_pixelID->wafer_hash_max()),m_moduleROD_errors[i]));
      cont->push_back(err);
    }
  }

  SG::WriteHandle<InDetBSErrContainer> errCont (m_BSErrContWriteKey);
  ATH_CHECK( errCont.record (std::move (cont)) );
  return StatusCode::SUCCESS;
}

void PixelByteStreamErrorsTool::addLinkMaskedByPPC() const {
  ++m_numLinkMaskedByPPC;
}

void PixelByteStreamErrorsTool::addLimitError() const {
  ++m_numLimitError;
}

void PixelByteStreamErrorsTool::updateServiceRecords(int code, unsigned int count) const {
  m_ServiceRecords[code] = count;
}

unsigned int PixelByteStreamErrorsTool::getServiceRecordCount(int code) const {
  return m_ServiceRecords[code];
}

