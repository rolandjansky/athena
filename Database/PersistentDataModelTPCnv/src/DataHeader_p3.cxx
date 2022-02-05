/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PersistentDataModelTPCnv/DataHeader_p3.h"

DataHeaderElement_p3::DataHeaderElement_p3() : m_clids(),
	m_token(),
	m_alias(),
	m_technology(0U),
	m_oid1(0U),
	m_oid2(0U),
	m_dbGuidIdx(0U),
	m_classIdIdx(0U),
	m_prefixIdx(0U),
	m_keyPos(0U),
	m_hashes() {}
DataHeaderElement_p3::DataHeaderElement_p3(const DataHeaderElement_p3& rhs) : m_clids(rhs.m_clids),
	m_token(rhs.m_token),
	m_alias(rhs.m_alias),
	m_technology(rhs.m_technology),
	m_oid1(rhs.m_oid1),
	m_oid2(rhs.m_oid2),
	m_dbGuidIdx(rhs.m_dbGuidIdx),
	m_classIdIdx(rhs.m_classIdIdx),
	m_prefixIdx(rhs.m_prefixIdx),
	m_keyPos(rhs.m_keyPos),
	m_hashes(rhs.m_hashes) {}
DataHeaderElement_p3::~DataHeaderElement_p3() {}

DataHeaderElement_p3& DataHeaderElement_p3::operator=(const DataHeaderElement_p3& rhs) {
   if (this != &rhs) {
      m_clids = rhs.m_clids;
      m_token = rhs.m_token;
      m_alias = rhs.m_alias;
      m_technology  = rhs.m_technology;
      m_oid1 = rhs.m_oid1;
      m_oid2 = rhs.m_oid2;
      m_dbGuidIdx = rhs.m_dbGuidIdx;
      m_classIdIdx = rhs.m_classIdIdx;
      m_keyPos = rhs.m_keyPos;
      m_hashes = rhs.m_hashes;
      m_prefixIdx = rhs.m_prefixIdx;
   }
   return(*this);
}

const std::vector<unsigned int>& DataHeaderElement_p3::clids() const {
   return(m_clids);
}

const std::string& DataHeaderElement_p3::token() const {
   return(m_token);
}

const std::vector<std::string> DataHeaderElement_p3::alias() const {
   return(m_alias);
}

const std::string& DataHeaderElement_p3::key() const {
   return(m_alias.front());
}

unsigned int DataHeaderElement_p3::pClid() const {
   return(m_clids.front());
}

unsigned int DataHeaderElement_p3::oid1() const {
   return(m_oid1);
}

unsigned int DataHeaderElement_p3::oid2() const {
   return(m_oid2);
}


DataHeader_p3::DataHeader_p3()
	: m_DataHeader(), m_InputDataHeader(), m_GuidMap() {}
DataHeader_p3::DataHeader_p3(const DataHeader_p3& rhs) : m_DataHeader(rhs.m_DataHeader),
	m_InputDataHeader(rhs.m_InputDataHeader),
	m_GuidMap(rhs.m_GuidMap) {}
DataHeader_p3::~DataHeader_p3() {}

DataHeader_p3& DataHeader_p3::operator=(const DataHeader_p3& rhs) {
   if (this != &rhs) {
      m_DataHeader = rhs.m_DataHeader;
      m_InputDataHeader = rhs.m_InputDataHeader;
      m_GuidMap = rhs.m_GuidMap;
   }
   return(*this);
}

const std::vector<DataHeaderElement_p3>& DataHeader_p3::elements() const {
   return(m_DataHeader);
}

const std::vector<DataHeaderElement_p3>& DataHeader_p3::inputElements() const {
   return(m_InputDataHeader);
}

const std::vector<std::string>& DataHeader_p3::GuidMap() const {
   return(m_GuidMap);
}
