/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file DataHeader.cxx
 *  @brief This file contains the implementation for the DataHeader and DataHeaderElement classes.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "PersistentDataModel/DataHeader.h"
#include "PersistentDataModel/TokenAddress.h"

#include "SGTools/TransientAddress.h"
#include "AthenaKernel/IStringPool.h"

//______________________________________________________________________________
DataHeaderElement::DataHeaderElement() : m_pClid(0), m_clids(), m_key(), m_alias(), m_token(0), m_ownToken(false), m_hashes() {
}
//______________________________________________________________________________
DataHeaderElement::DataHeaderElement(const DataHeaderElement& rhs) : m_pClid(rhs.m_pClid),
	m_clids(rhs.m_clids),
	m_key(rhs.m_key),
	m_alias(rhs.m_alias),
	m_token(0),
	m_ownToken(false),
	m_hashes(rhs.m_hashes) {
   if (rhs.getToken() != 0) {
      m_token = new Token(rhs.getToken()); m_ownToken = true;
   }
}
//______________________________________________________________________________
DataHeaderElement::DataHeaderElement(const SG::TransientAddress* sgAddress,
	const std::string& pTag) : m_pClid(0), m_clids(), m_key(), m_alias(), m_token(0), m_ownToken(false), m_hashes() {
   if (sgAddress != 0) {
      m_pClid = sgAddress->clID();
      std::set<CLID> tClids = sgAddress->transientID();
      std::set<CLID>::iterator lastClid = m_clids.begin();
      for (std::set<CLID>::const_iterator iter = tClids.begin(), last = tClids.end();
	      iter != last; iter++) {
         lastClid = m_clids.insert(lastClid, *iter);
      }
      m_clids.erase(m_pClid);
      m_key = (pTag.empty()) ? sgAddress->name() : pTag;
      m_alias = sgAddress->alias();
      TokenAddress* tokAddr = dynamic_cast<TokenAddress*>(sgAddress->address());
      if (tokAddr == 0 || tokAddr->getToken() == 0) {
         m_token = new Token; m_ownToken = true;
         const_cast<Token*>(m_token)->fromString(*(sgAddress->address()->par()));
      } else {
         m_token = tokAddr->getToken();
      }
   }
}
//______________________________________________________________________________
DataHeaderElement::DataHeaderElement(const CLID classID,
	const std::string& key, const Token* token)
	: m_pClid(classID), m_clids(), m_key(key), m_alias(), m_token(token), m_ownToken(true), m_hashes() {
}
//______________________________________________________________________________
DataHeaderElement::~DataHeaderElement() {
   if (m_ownToken) { delete m_token; m_token = 0; }
}
//______________________________________________________________________________
DataHeaderElement& DataHeaderElement::operator=(const DataHeaderElement& rhs) {
   if (this != &rhs) {
      m_pClid = rhs.m_pClid;
      m_clids = rhs.m_clids;
      m_key = rhs.m_key;
      m_alias = rhs.m_alias;
      if (m_ownToken) { delete m_token; m_token = 0; m_ownToken = false; }
      if (rhs.getToken() != 0) {
         m_token = new Token; m_ownToken = true;
         rhs.getToken()->setData(const_cast<Token*>(m_token));
      }
      m_hashes = rhs.m_hashes;
   }
   return(*this);
}
//______________________________________________________________________________
CLID DataHeaderElement::getPrimaryClassID() const {
   if (m_pClid > 0) {
      return(m_pClid);
   }
   return(*(m_clids.begin()));
}
//______________________________________________________________________________
const std::set<CLID> DataHeaderElement::getClassIDs() const {
   std::set<CLID> allClids;
   std::set<CLID>::iterator lastClid = allClids.begin();
   for (std::set<CLID>::const_iterator iter = m_clids.begin(), last = m_clids.end();
	   iter != last; iter++) {
      lastClid = allClids.insert(lastClid, *iter);
   }
   allClids.insert(m_pClid);
   return(allClids);
}
//______________________________________________________________________________
const std::string& DataHeaderElement::getKey() const {
   return(m_key);
}
//______________________________________________________________________________
const std::set<std::string>& DataHeaderElement::getAlias() const {
   return(m_alias);
}
//_____________________________________________________________________________
const Token* DataHeaderElement::getToken() const {
   return(m_token);
}
//_____________________________________________________________________________
long DataHeaderElement::getStorageType() const {
   return(POOL_StorageType);
}
//_____________________________________________________________________________
const std::vector<unsigned int>& DataHeaderElement::getHashes() const {
   return(m_hashes);
}
//______________________________________________________________________________
void DataHeaderElement::addHash(IStringPool* pool) {
   if (!m_hashes.empty()) {
      return;
   }
   std::set<CLID> clids = getClassIDs();
   for (std::set<CLID>::iterator iter = clids.begin(), last = clids.end(); iter != last; iter++) {
      m_hashes.push_back(pool->stringToKey(m_key, *iter));
   }
}
//______________________________________________________________________________
SG::TransientAddress* DataHeaderElement::getAddress(unsigned long contextId) const {
   return(getAddress(m_key, contextId));
}
//______________________________________________________________________________
SG::TransientAddress* DataHeaderElement::getAddress(const std::string& key,
	unsigned long contextId) const {
   CLID primaryClID = getPrimaryClassID();
   Token* token = new Token(m_token);
   TokenAddress* tokAdd = new TokenAddress(this->getStorageType(), primaryClID, "", m_key, contextId , token);
   SG::TransientAddress* sgAddress = new SG::TransientAddress(primaryClID, key, tokAdd);
   for (std::set<CLID>::const_iterator iter = m_clids.begin(), last = m_clids.end();
	   iter != last; iter++) {
      sgAddress->setTransientID(*iter);
   }
   sgAddress->setAlias(m_alias);
   return(sgAddress);
}
//______________________________________________________________________________
//______________________________________________________________________________
DataHeader::DataHeader() : m_dataHeader(),
	m_inputDataHeader(),
	m_status(statusFlag(Other)),
	m_processTag(),
	m_attrList(0),
	m_evtRefTokenStr() {
}
//______________________________________________________________________________
DataHeader::DataHeader(const DataHeader& rhs) : m_dataHeader(rhs.m_dataHeader),
	m_inputDataHeader(rhs.m_inputDataHeader),
	m_status(rhs.m_status),
	m_processTag(rhs.m_processTag),
	m_attrList(rhs.m_attrList),
	m_evtRefTokenStr() {
}
//______________________________________________________________________________
DataHeader::~DataHeader() {
}
//______________________________________________________________________________
DataHeader& DataHeader::operator=(const DataHeader& rhs) {
   if (this != &rhs) {
      m_dataHeader = rhs.m_dataHeader;
      m_inputDataHeader = rhs.m_inputDataHeader;
      m_status = rhs.m_status;
      m_processTag = rhs.m_processTag;
      m_attrList = rhs.m_attrList;
   }
   return(*this);
}
//______________________________________________________________________________
void DataHeader::setStatus(statusFlag status) const {
   m_status = status;
}
//______________________________________________________________________________
bool DataHeader::checkStatus(statusFlag status) const {
   return(m_status == status);
}
//______________________________________________________________________________
bool DataHeader::isInput() const {
   return(m_status == statusFlag(Primary) || m_status == statusFlag(Input));
}
//______________________________________________________________________________
void DataHeader::setProcessTag(const std::string& processTag) const {
   m_processTag = processTag;
}
//______________________________________________________________________________
const std::string& DataHeader::getProcessTag() const {
   return(m_processTag);
}
//______________________________________________________________________________
int DataHeader::size() const {
   return(m_dataHeader.size());
}
//______________________________________________________________________________
std::vector<DataHeaderElement>::const_iterator DataHeader::begin() const {
   return(m_dataHeader.begin());
}
//______________________________________________________________________________
std::vector<DataHeaderElement>::const_iterator DataHeader::end() const {
   return(m_dataHeader.end());
}
//______________________________________________________________________________
const std::vector<DataHeaderElement>& DataHeader::elements() const {
   return m_dataHeader;
}
//______________________________________________________________________________
void DataHeader::insert(const SG::TransientAddress* sgAddress, const std::string& pTag) {
   if (sgAddress != 0) {
      DataHeaderElement dhElement(sgAddress, pTag);
      m_dataHeader.push_back(dhElement);
   }
}
//______________________________________________________________________________
void DataHeader::insert(const DataHeaderElement& dhe) {
   m_dataHeader.push_back(dhe);
}
//______________________________________________________________________________
int DataHeader::sizeProvenance() const {
   return(m_inputDataHeader.size());
}
//______________________________________________________________________________
std::vector<DataHeaderElement>::const_iterator DataHeader::beginProvenance() const {
   return(m_inputDataHeader.begin());
}
//______________________________________________________________________________
std::vector<DataHeaderElement>::const_iterator DataHeader::endProvenance() const {
   return(m_inputDataHeader.end());
}
//______________________________________________________________________________
void DataHeader::insertProvenance(const DataHeaderElement& dhe) {
   m_inputDataHeader.push_back(dhe);
}
//______________________________________________________________________________
void DataHeader::addHash(IStringPool* pool) {
   for (std::vector<DataHeaderElement>::iterator iter = m_dataHeader.begin(),
		   last = m_dataHeader.end(); iter != last; iter++) {
      iter->addHash(pool);
   }
   for (std::vector<DataHeaderElement>::iterator iter = m_inputDataHeader.begin(),
		   last = m_inputDataHeader.end(); iter != last; iter++) {
      iter->addHash(pool);
   }
}
//______________________________________________________________________________
void DataHeader::setAttributeList(const coral::AttributeList* attrList) {
   m_attrList = attrList;
}
//______________________________________________________________________________
const coral::AttributeList* DataHeader::getAttributeList() {
   return(m_attrList);
}
//______________________________________________________________________________
void DataHeader::setEvtRefTokenStr(const std::string& tokenStr) {
   m_evtRefTokenStr = tokenStr;
}
//______________________________________________________________________________
const std::string& DataHeader::getEvtRefTokenStr() {
   return(m_evtRefTokenStr);
}
