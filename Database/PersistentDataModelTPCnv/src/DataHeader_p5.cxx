/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PersistentDataModelTPCnv/DataHeader_p5.h"

#include "CxxUtils/MD5.h"

#include <uuid/uuid.h>
#include <sstream>

DataHeaderElement_p5::DataHeaderElement_p5() : m_token(), m_oid2(0U) {}
DataHeaderElement_p5::DataHeaderElement_p5(const DataHeaderElement_p5& rhs) : m_token(rhs.m_token),
	m_oid2(rhs.m_oid2) {}
DataHeaderElement_p5::~DataHeaderElement_p5() {}

DataHeaderElement_p5& DataHeaderElement_p5::operator=(const DataHeaderElement_p5& rhs) {
   if (this != &rhs) {
      m_token = rhs.m_token;
      m_oid2 = rhs.m_oid2;
   }
   return(*this);
}

const std::string& DataHeaderElement_p5::token() const {
   return(m_token);
}

unsigned int DataHeaderElement_p5::oid2() const {
   return(m_oid2);
}

void DataHeaderElement_p5::overwriteOid2(unsigned int oid2) {
   m_oid2 = oid2;
}


DataHeaderForm_p5::DataHeaderForm_p5() : m_map(), m_uints(), m_entry(0U) {}
DataHeaderForm_p5::DataHeaderForm_p5(const DataHeaderForm_p5& rhs) : m_map(rhs.m_map), m_uints(rhs.m_uints), m_entry(0U) {}
DataHeaderForm_p5::~DataHeaderForm_p5() {}
DataHeaderForm_p5& DataHeaderForm_p5::operator=(const DataHeaderForm_p5& rhs) {
   if (&rhs != this) {
      m_map = rhs.m_map;
      m_uints = rhs.m_uints;
      m_entry = 0U;
   }
   return(*this);
}

const std::vector<std::string>& DataHeaderForm_p5::map() const {
   return(m_map);
}

void DataHeaderForm_p5::insertMap(const std::string& element) {
   m_map.push_back(element);
}

const std::vector<unsigned int>& DataHeaderForm_p5::params() const {
   return(m_uints[m_entry - 1]);
}

void DataHeaderForm_p5::insertParam(unsigned int param) {
   m_uints[m_entry - 1].push_back(param);
}

unsigned int DataHeaderForm_p5::entry() const {
   return(m_entry);
}

void DataHeaderForm_p5::start() const {
   m_entry = 1;
}

void DataHeaderForm_p5::next() const {
   m_entry++;
}

unsigned int DataHeaderForm_p5::size() {
   return(m_uints.size());
}

void DataHeaderForm_p5::resize(unsigned int size) {
   m_uints.resize(size);
}


DataHeader_p5::DataHeader_p5() : m_dataHeader(), m_dhForm(0), m_dhFormToken(), m_dhFormMdx() {}
DataHeader_p5::DataHeader_p5(const DataHeader_p5& rhs) : m_dataHeader(rhs.m_dataHeader),
	m_dhForm(0),
	m_dhFormToken(rhs.m_dhFormToken),
	m_dhFormMdx(rhs.m_dhFormMdx) {}
DataHeader_p5::~DataHeader_p5() {
   delete m_dhForm;
}

DataHeader_p5& DataHeader_p5::operator=(const DataHeader_p5& rhs) {
   if (this != &rhs) {
      m_dataHeader = rhs.m_dataHeader;
      m_dhForm = 0;
      m_dhFormToken = rhs.m_dhFormToken;
      m_dhFormMdx = rhs.m_dhFormMdx;
   }
   return(*this);
}

const std::vector<DataHeaderElement_p5>& DataHeader_p5::elements() const {
   return(m_dataHeader);
}

const DataHeaderForm_p5* DataHeader_p5::dhForm() const {
   return(m_dhForm);
}

void DataHeader_p5::setDhForm(DataHeaderForm_p5* form) {
   m_dhForm = form;
}

const std::string& DataHeader_p5::dhFormToken() const {
   return(m_dhFormToken);
}

void DataHeader_p5::setDhFormToken(const std::string& formToken) {
   if (m_dhForm) {
      m_dhFormToken = formToken;
      std::ostringstream stream;
      for (std::vector<std::string>::const_iterator iter = m_dhForm->map().begin(),
		      last = m_dhForm->map().end(); iter != last; iter++) {
         stream << *iter << "\n";
      }
      for (m_dhForm->start(); m_dhForm->entry() <= m_dhForm->size(); m_dhForm->next()) {
         for (std::vector<unsigned int>::const_iterator iter = m_dhForm->params().begin(),
		         last = m_dhForm->params().end(); iter != last; iter++) {
            stream << *iter << ",";
         }
         stream << "\n";
      }
      MD5 checkSum((unsigned char*)stream.str().c_str(), stream.str().size());
      uuid_t checkSumUuid;
      checkSum.raw_digest((unsigned char*)(&checkSumUuid));
      char text[37];
      uuid_unparse_upper(checkSumUuid, text);
      m_dhFormMdx = text;
   }
}
const std::string& DataHeader_p5::dhFormMdx() const {
   return(m_dhFormMdx);
}
