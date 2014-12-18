/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PersistentDataModelTPCnv/DataHeader_p6.h"

#include "CxxUtils/MD5.h"

#include <uuid/uuid.h>
#include <sstream>

DataHeaderElement_p6::DataHeaderElement_p6() :
  m_token(),
  m_oid1(0ULL),
  m_oid2(0ULL),
  m_dbIdx(0),
  m_objIdx(0) {}
DataHeaderElement_p6::DataHeaderElement_p6(const DataHeaderElement_p6& rhs) : m_token(rhs.m_token),
	m_oid1(rhs.m_oid1),
	m_oid2(rhs.m_oid2),
	m_dbIdx(rhs.m_dbIdx),
	m_objIdx(rhs.m_objIdx) {}
DataHeaderElement_p6::~DataHeaderElement_p6() {}

DataHeaderElement_p6& DataHeaderElement_p6::operator=(const DataHeaderElement_p6& rhs) {
   if (this != &rhs) {
      m_token = rhs.m_token;
      m_oid1 = rhs.m_oid1;
      m_oid2 = rhs.m_oid2;
      m_dbIdx = rhs.m_dbIdx;
      m_objIdx = rhs.m_objIdx;
   }
   return(*this);
}

const std::string& DataHeaderElement_p6::token() const {
   return(m_token);
}

unsigned long long DataHeaderElement_p6::oid1() const {
   return(m_oid1);
}

unsigned long long DataHeaderElement_p6::oid2() const {
   return(m_oid2);
}

void DataHeaderElement_p6::overwriteOid2(unsigned long long oid2) {
   m_oid2 = oid2;
}


DataHeaderForm_p6::DataHeaderForm_p6() : m_uints(), m_dbRecords(), m_objRecords(), m_objAlias(), m_objSymLinks(), m_objHashes(), m_entry(0U) {}
DataHeaderForm_p6::DataHeaderForm_p6(const DataHeaderForm_p6& rhs) : m_uints(rhs.m_uints), m_dbRecords(rhs.m_dbRecords), m_objRecords(rhs.m_objRecords), m_objAlias(rhs.m_objAlias), m_objSymLinks(rhs.m_objSymLinks), m_objHashes(rhs.m_objHashes), m_entry(0U) {}
DataHeaderForm_p6::~DataHeaderForm_p6() {}
DataHeaderForm_p6& DataHeaderForm_p6::operator=(const DataHeaderForm_p6& rhs) {
   if (&rhs != this) {
      m_uints = rhs.m_uints;
      m_dbRecords = rhs.m_dbRecords;
      m_objRecords = rhs.m_objRecords;
      m_objAlias = rhs.m_objAlias;
      m_objSymLinks = rhs.m_objSymLinks;
      m_objHashes = rhs.m_objHashes;
      m_entry = 0U;
   }
   return(*this);
}

unsigned int DataHeaderForm_p6::insertDb(const DbRecord& rec) {
   unsigned int index = 0U;
   for (std::vector<DbRecord>::const_iterator iter = m_dbRecords.begin(), last = m_dbRecords.end();
	   iter != last; iter++, index++) {
      if (*iter == rec) break;
   }
   if (index == m_dbRecords.size()) {
      m_dbRecords.push_back(rec);
   }
   return(index);
}

std::size_t DataHeaderForm_p6::sizeDb() const {
   return(m_dbRecords.size());
}

Guid DataHeaderForm_p6::getDbGuid(unsigned int index) const {
   return(m_dbRecords[index].first);
}

unsigned int DataHeaderForm_p6::getDbTech(unsigned int index) const {
   return(m_dbRecords[index].second);
}

unsigned int DataHeaderForm_p6::insertObj(const ObjRecord& rec,
	const std::set<std::string>& alias,
	const std::set<unsigned int>& symLinks,
	const std::vector<unsigned int>& hashes) {
   unsigned int index = 0U;
   for (std::vector<ObjRecord>::const_iterator iter = m_objRecords.begin(), last = m_objRecords.end();
           iter != last; iter++, index++) {
      if (*iter == rec) break;
   }
   if (index == m_objRecords.size()) {
      m_objRecords.push_back(rec);
      m_objAlias.push_back(std::vector<std::string>(alias.begin(), alias.end()));
      m_objSymLinks.push_back(std::vector<unsigned int>(symLinks.begin(), symLinks.end()));
      m_objHashes.push_back(hashes);
   }
   return(index);
}

std::size_t DataHeaderForm_p6::sizeObj() const {
   return(m_objRecords.size());
}

std::string DataHeaderForm_p6::getObjKey(unsigned int index) const {
   return(m_objRecords[index].second.first);
}

unsigned int DataHeaderForm_p6::getObjType(unsigned int index) const {
   return(m_objRecords[index].second.second);
}

Guid DataHeaderForm_p6::getObjClassId(unsigned int index) const {
   return(m_objRecords[index].first);
}

std::set<std::string> DataHeaderForm_p6::getObjAlias(unsigned int index) const {
   return(std::set<std::string>(m_objAlias[index].begin(), m_objAlias[index].end()));
}

std::set<unsigned int> DataHeaderForm_p6::getObjSymLinks(unsigned int index) const {
   return(std::set<unsigned int>(m_objSymLinks[index].begin(), m_objSymLinks[index].end()));
}

std::vector<unsigned int> DataHeaderForm_p6::getObjHashes(unsigned int index) const {
   return(m_objHashes[index]);
}



const std::vector<unsigned int>& DataHeaderForm_p6::params() const {
   return(m_uints[m_entry]);
}

void DataHeaderForm_p6::insertParam(unsigned int param) {
   m_uints[m_entry].push_back(param);
}

unsigned int DataHeaderForm_p6::entry() const {
   return(m_entry);
}

void DataHeaderForm_p6::start() const {
   m_entry = 0;
}

void DataHeaderForm_p6::next() const {
   m_entry++;
}

unsigned int DataHeaderForm_p6::size() const {
   return(m_uints.size());
}

void DataHeaderForm_p6::resize(unsigned int size) {
   m_uints.resize(size);
}


DataHeader_p6::DataHeader_p6() : m_dataHeader(), m_provenanceSize(0U), m_dhForm(), m_dhFormToken(), m_dhFormMdx() {}
DataHeader_p6::DataHeader_p6(const DataHeader_p6& rhs) :
  m_dataHeader(rhs.m_dataHeader),
  m_provenanceSize(rhs.m_provenanceSize),
  m_dhForm(rhs.m_dhForm),
  m_dhFormToken(rhs.m_dhFormToken),
  m_dhFormMdx(rhs.m_dhFormMdx) {}
DataHeader_p6::~DataHeader_p6() {}

DataHeader_p6& DataHeader_p6::operator=(const DataHeader_p6& rhs) {
   if (this != &rhs) {
      m_dataHeader = rhs.m_dataHeader;
      m_provenanceSize = rhs.m_provenanceSize;
      m_dhForm = rhs.m_dhForm;
      m_dhFormToken = rhs.m_dhFormToken;
      m_dhFormMdx = rhs.m_dhFormMdx;
   }
   return(*this);
}

const std::vector<DataHeaderElement_p6>& DataHeader_p6::elements() const {
   return(m_dataHeader);
}

const DataHeaderForm_p6& DataHeader_p6::dhForm() const {
   return(m_dhForm);
}

void DataHeader_p6::setDhForm(const DataHeaderForm_p6& form) {
   m_dhForm = form;
}

const std::string& DataHeader_p6::dhFormToken() const {
   return(m_dhFormToken);
}

void DataHeader_p6::setDhFormToken(const std::string& formToken) {
   m_dhFormToken = formToken;
}

void DataHeader_p6::calculateDhFormMdx() {
   std::ostringstream stream;
   for (std::size_t iter = 0, last = m_dhForm.sizeDb(); iter != last; iter++) {
	stream << m_dhForm.getDbGuid(iter) << "," << m_dhForm.getDbTech(iter) << "\n";
   }
   for (std::size_t iter = 0, last = m_dhForm.sizeObj(); iter != last; iter++) {
	stream << m_dhForm.getObjKey(iter) << "," << m_dhForm.getObjType(iter) << "," << m_dhForm.getObjClassId(iter) << "\n";
   }
   MD5 checkSum((unsigned char*)stream.str().c_str(), stream.str().size());
   uuid_t checkSumUuid;
   checkSum.raw_digest((unsigned char*)(&checkSumUuid));
   char text[37];
   uuid_unparse_upper(checkSumUuid, text);
   m_dhFormMdx = text;
}

const std::string& DataHeader_p6::dhFormMdx() const {
   return(m_dhFormMdx);
}
