/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file DataHeaderCnv_p3.cxx
 *  @brief This file contains the implementation for the DataHeaderCnv_p3 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: DataHeaderCnv_p3.cxx,v 1.6 2009-05-15 01:01:57 gemmeren Exp $
 **/

#include "PersistentDataModel/DataHeader.h"
#include "PersistentDataModelTPCnv/DataHeaderCnv_p3.h"

#include <cstdio>
#include <sstream>

DataHeaderElementCnv_p3::DataHeaderElementCnv_p3() {}
DataHeaderElementCnv_p3::~DataHeaderElementCnv_p3() {}

//______________________________________________________________________________
void DataHeaderElementCnv_p3::persToTrans(const DataHeaderElement_p3* pers,
	DataHeaderElement* trans,
	const std::vector<std::string>& map) {
   trans->m_pClid = *pers->m_clids.begin();
   trans->m_clids.clear();
   std::set<CLID>::iterator lastClid = trans->m_clids.begin();
   for (std::vector<unsigned int>::const_iterator iter = pers->m_clids.begin() + 1,
		   last = pers->m_clids.end(); iter != last; ++iter) {
      lastClid = trans->m_clids.insert(lastClid, *iter);
   }
   trans->m_key = *pers->m_alias.begin();
   trans->m_alias.clear();
   std::set<std::string>::iterator lastAlias = trans->m_alias.begin();
   for (std::vector<std::string>::const_iterator iter = pers->m_alias.begin() + 1,
		   last = pers->m_alias.end(); iter != last; ++iter) {
      lastAlias = trans->m_alias.insert(lastAlias, *iter);
   }
   trans->m_hashes.clear();
   trans->m_hashes.reserve(pers->m_hashes.size());
   for (std::vector<uint64_t>::const_iterator iter = pers->m_hashes.begin(),
		   last = pers->m_hashes.end(); iter != last; ++iter) {
      trans->m_hashes.push_back((unsigned int)(*iter));
   }
// Translate PoolToken
   std::string tokenStr;
   if (map.empty()) {
      tokenStr = pers->m_token;
   } else {
      unsigned int tokenSize = 129 + pers->m_token.size();
      if (pers->m_prefixIdx > 0) {
         tokenSize += map[pers->m_prefixIdx].size();
      }
      if (pers->m_keyPos > 0) {
         tokenSize += trans->m_key.size();
      }
      tokenStr.reserve(tokenSize);
// Append DbGuid
      tokenStr = "[DB=";
      tokenStr.append(map[pers->m_dbGuidIdx]);
// Still optimize
      tokenStr.append("][CNT=");
      if (pers->m_prefixIdx > 0) {
         tokenStr.append(map[pers->m_prefixIdx]);
      }
// insert key
      if (pers->m_keyPos > 0) {
         tokenStr.append(pers->m_token.substr(0, pers->m_keyPos - 1));
         tokenStr.append(trans->m_key);
         tokenStr.append(pers->m_token.substr(pers->m_keyPos - 1));
      } else {
         tokenStr.append(pers->m_token);
      }
// Append ClassId
      tokenStr.append("][CLID=");
      tokenStr.append(map[pers->m_classIdIdx]);
      const unsigned int length = 40;
      char text[length];
      snprintf(text, length, "][TECH=%08X][OID=%08X-%08X]", pers->m_technology, pers->m_oid1, pers->m_oid2);
      tokenStr.append(text);
   }
   Token* token = new Token;
   token->fromString(tokenStr);
   delete trans->m_token; trans->m_token = token;
}
//______________________________________________________________________________
void DataHeaderElementCnv_p3::transToPers(const DataHeaderElement* /*trans*/,
	DataHeaderElement_p3* /*pers*/,
	std::vector<std::string>& /*map*/) {
// Moved to next _p version
}
//______________________________________________________________________________
//______________________________________________________________________________
DataHeaderCnv_p3::DataHeaderCnv_p3() {}
DataHeaderCnv_p3::~DataHeaderCnv_p3() {}
//______________________________________________________________________________
void DataHeaderCnv_p3::persToTrans(const DataHeader_p3* pers, DataHeader* trans) {
   trans->m_dataHeader.resize(pers->m_DataHeader.size());
   std::vector<DataHeaderElement>::iterator it = trans->m_dataHeader.begin();
   for (std::vector<DataHeaderElement_p3>::const_iterator pit = pers->m_DataHeader.begin(),
		   last = pers->m_DataHeader.end(); pit != last; ++it, ++pit) {
      m_elemCnv.persToTrans(&(*pit), &(*it), pers->m_GuidMap);
   }
   trans->m_inputDataHeader.resize(pers->m_InputDataHeader.size());
   it = trans->m_inputDataHeader.begin();
   for (std::vector<DataHeaderElement_p3>::const_iterator pit = pers->m_InputDataHeader.begin(),
		   last = pers->m_InputDataHeader.end(); pit != last; ++it, ++pit) {
      m_elemCnv.persToTrans(&(*pit), &(*it), pers->m_GuidMap);
   }
}
//______________________________________________________________________________
void DataHeaderCnv_p3::transToPers(const DataHeader* /*trans*/, DataHeader_p3* /*pers*/) {
// Moved to next _p version
}
//______________________________________________________________________________
void DataHeaderCnv_p3::insertDHRef(DataHeader_p3* /*pers*/,
	const std::string& /*key*/,
	const std::string& /*token*/) {
// Moved to next _p version
}
