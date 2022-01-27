/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PersistentDataModelTPCnv/DataHeader_p4.h"

DataHeaderElement_p4::DataHeaderElement_p4() : m_clids(),
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
DataHeaderElement_p4::DataHeaderElement_p4(const DataHeaderElement_p4& rhs) : m_clids(rhs.m_clids),
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
DataHeaderElement_p4::~DataHeaderElement_p4() {}

DataHeaderElement_p4& DataHeaderElement_p4::operator=(const DataHeaderElement_p4& rhs) {
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

unsigned int DataHeaderElement_p4::pClid() const {
   return(m_clids.front());
}

const std::vector<unsigned int>& DataHeaderElement_p4::clids() const {
   return(m_clids);
}

const std::string& DataHeaderElement_p4::key() const {
   return(m_alias.front());
}

const std::vector<std::string>& DataHeaderElement_p4::alias() const {
   return(m_alias);
}

const std::string& DataHeaderElement_p4::token() const {
   return(m_token);
}

unsigned int DataHeaderElement_p4::oid1() const {
   return(m_oid1);
}

unsigned int DataHeaderElement_p4::oid2() const {
   return(m_oid2);
}


DataHeader_p4::DataHeader_p4() : m_dataHeader(), m_provSize(0U), m_guidMap() {}
DataHeader_p4::DataHeader_p4(const DataHeader_p4& rhs) : m_dataHeader(rhs.m_dataHeader),
	m_provSize(rhs.m_provSize),
	m_guidMap(rhs.m_guidMap) {}
DataHeader_p4::~DataHeader_p4() {}

DataHeader_p4& DataHeader_p4::operator=(const DataHeader_p4& rhs) {
   if (this != &rhs) {
      m_dataHeader = rhs.m_dataHeader;
      m_provSize = rhs.m_provSize;
      m_guidMap = rhs.m_guidMap;
   }
   return(*this);
}

const std::vector<DataHeaderElement_p4>& DataHeader_p4::elements() const {
   return(m_dataHeader);
}

unsigned int DataHeader_p4::provenanceSize() const {
   return(m_provSize);
}

const std::vector<std::string>& DataHeader_p4::guidMap() const {
   return(m_guidMap);
}
