/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PersistentDataModelTPCnv/DataHeader_p6.h"
#include "PersistentDataModel/Token.h"

#include "CxxUtils/MD5.h"

#include <uuid/uuid.h>

DataHeaderForm_p6::DataHeaderForm_p6(const DataHeaderForm_p6& rhs) :
      m_dbRecords(rhs.m_dbRecords), m_objRecords(rhs.m_objRecords),
      m_objAlias(rhs.m_objAlias), m_objSymLinks(rhs.m_objSymLinks),
      m_objHashes(rhs.m_objHashes), m_version(rhs.m_version),
      m_modified(rhs.m_modified), m_token(nullptr)
{
   setToken(rhs.m_token);
}

DataHeaderForm_p6& DataHeaderForm_p6::operator=(const DataHeaderForm_p6& rhs) {
   if (&rhs != this) {
      m_dbRecords = rhs.m_dbRecords;
      m_objRecords = rhs.m_objRecords;
      m_objAlias = rhs.m_objAlias;
      m_objSymLinks = rhs.m_objSymLinks;
      m_objHashes = rhs.m_objHashes;
      m_version = rhs.m_version;
      m_modified = rhs.m_modified;
      setToken(rhs.m_token);
   }
   return(*this);
}

DataHeaderForm_p6::~DataHeaderForm_p6()
{
   if( m_token ) m_token->release();
}


unsigned int DataHeaderForm_p6::insertDb(const DbRecord& rec) {
   unsigned int index = 0U;
   for (std::vector<DbRecord>::const_iterator iter = m_dbRecords.begin(), last = m_dbRecords.end();
	   iter != last; iter++, index++) {
      if (*iter == rec) break;
   }
   if (index == m_dbRecords.size()) {
      m_dbRecords.push_back(rec);
      m_modified = true;
   }
   return(index);
}

std::size_t DataHeaderForm_p6::sizeDb() const {
   return(m_dbRecords.size());
}

Guid DataHeaderForm_p6::getDbGuid(unsigned int index) const {
   return m_dbRecords[index].fid;
}

unsigned int DataHeaderForm_p6::getDbTech(unsigned int index) const {
   return m_dbRecords[index].tech;
}

unsigned int DataHeaderForm_p6::insertObj(const ObjRecord& rec,
	const std::set<std::string>& alias,
	const std::set<unsigned int>& symLinks,
	const std::vector<unsigned int>& hashes)
{
   unsigned int index = 0U;
   for (std::vector<ObjRecord>::const_iterator iter = m_objRecords.begin(), last = m_objRecords.end();
           iter != last; iter++, index++) {
      if (*iter == rec) break;
   }
   std::vector<std::string>     aliases( alias.begin(), alias.end() );
   std::vector<unsigned int>    symlinks( symLinks.begin(), symLinks.end() );
   if (index != m_objRecords.size()) {
      // found matching object record, check if all the info is the same
      if( m_objAlias[index] != aliases ) {
         m_objAlias[index] = std::move(aliases);
         m_modified = true;
      }
      if( m_objSymLinks[index] != symlinks ) {
         m_objSymLinks[index] = std::move(symlinks);
         m_modified = true;
      }
      if( m_objHashes[index] != hashes ) {
         m_objHashes[index] = hashes;
         m_modified = true;
      }
      return index;
   }
   // enter a new record
   m_objRecords.push_back( rec );
   m_objAlias.push_back( std::move(aliases) );
   m_objSymLinks.push_back( std::move(symlinks) );
   m_objHashes.push_back( hashes );
   m_modified = true;
   return m_objRecords.size() - 1;
}

std::size_t DataHeaderForm_p6::sizeObj() const {
   return(m_objRecords.size());
}

std::string DataHeaderForm_p6::getObjKey(unsigned int index) const {
   return m_objRecords[index].key;
}

unsigned int DataHeaderForm_p6::getObjType(unsigned int index) const {
   return m_objRecords[index].clid;
}

Guid DataHeaderForm_p6::getObjClassId(unsigned int index) const {
   return m_objRecords[index].guid;
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


bool DataHeaderForm_p6::wasModified() const {
   return m_modified;
}

void DataHeaderForm_p6::clearModified() {
   m_modified = false;
}

void DataHeaderForm_p6::setToken( Token *tok )
{
   if( tok ) tok->addRef(); 
   if( m_token ) m_token->release();
   m_token = tok;
}

Token* DataHeaderForm_p6::getToken() const {
   return m_token;
}

void DataHeaderForm_p6::resize(unsigned int size) {
   m_objRecords.reserve( size );
   m_objAlias.reserve( size );
   m_objSymLinks.reserve( size );
   m_objHashes.reserve( size );
}



//---------------------------------------------------------------------

const std::string& DataHeader_p6::dhFormToken() const {
   return(m_dhFormToken);
}

void DataHeader_p6::setDhFormToken(const std::string& formToken) {
   m_dhFormToken = formToken;
}

