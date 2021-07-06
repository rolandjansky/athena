/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file SqliteReadSvc.cxx
 *
 * @brief Implementation of SqliteReadSvc class
 *
 */

#include "SqliteReadSvc.h"
#include "SqliteRecordset.h"
#include <sqlite3.h>

#include "RDBQuery.h"

SqliteReadSvc::SqliteReadSvc(const std::string& name, ISvcLocator* svc)
  : AthService(name,svc)
{
}

StatusCode SqliteReadSvc::finalize()
{
  m_recordsets.clear();
  shutdown("");

  return StatusCode::SUCCESS;
}

StatusCode SqliteReadSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if (IID_IRDBAccessSvc == riid) {
    *ppvInterface = (IRDBAccessSvc*)this;
  }
  else { 
    return AthService::queryInterface(riid, ppvInterface);
  }

  addRef();
  return StatusCode::SUCCESS;
}

bool SqliteReadSvc::connect(const std::string& connName)
{
  std::lock_guard<std::mutex> guard(m_sessionMutex);
  if(!m_db) {
    int res = sqlite3_open(connName.c_str(), &m_db);
    if (res != SQLITE_OK) {
      ATH_MSG_FATAL("Failed to open " << connName << ". " << sqlite3_errmsg(m_db));
      return false;
    }
  }
  return true;
}

bool SqliteReadSvc::disconnect(const std::string&)
{
  // Dummy implementation
  return true;
}

bool SqliteReadSvc::shutdown(const std::string&)
{
  std::lock_guard<std::mutex> guard(m_sessionMutex);
  if(m_db) {
    sqlite3_close(m_db);
    m_db = nullptr;
  }
  return true;
}


IRDBRecordset_ptr SqliteReadSvc::getRecordsetPtr(const std::string& node
						, const std::string& 
						, const std::string& 
						, const std::string& )
{
  ATH_MSG_DEBUG("Getting RecordsetPtr with key " << node);

  std::lock_guard<std::mutex> guard(m_recordsetMutex);
  if(!m_db) {
    ATH_MSG_ERROR("Connection to the SQLite database not open. Returning empty recordset");
    return IRDBRecordset_ptr(new SqliteRecordset());
  }

  auto itRecordset = m_recordsets.find(node);
  if(itRecordset!=m_recordsets.end()) {
    ATH_MSG_DEBUG("Reusing an existing recordset");
    return itRecordset->second;
  }

  SqliteRecordset* recConcrete = new SqliteRecordset();
  recConcrete->getData(m_db,node);
  IRDBRecordset_ptr rec(recConcrete);
  m_recordsets.emplace(node,rec);
  return rec;
}

std::unique_ptr<IRDBQuery> SqliteReadSvc::getQuery(const std::string& node
						  , const std::string& tag
						  , const std::string& tag2node
						  , const std::string& connName)
{
  ATH_MSG_DEBUG("getQuery (" << node << "," << tag << "," << tag2node << "," << connName << ")");
  ATH_MSG_WARNING("SqliteReadSvc::getQuery is a dummy method");

  return std::unique_ptr<IRDBQuery>();
}

std::string SqliteReadSvc::getChildTag(const std::string& childNode
				      , const std::string& parentTag
				      , const std::string& parentNode
				      , const std::string& )
{
  ATH_MSG_DEBUG("getChildTag for " << childNode << " " << parentTag << " " << parentNode);
  ATH_MSG_WARNING("SqliteReadSvc::getChildTag is a dummy method");
  return std::string();
}

void SqliteReadSvc::getTagDetails(RDBTagDetails& 
                                 , const std::string& tag
                                 , const std::string& )
{
  ATH_MSG_DEBUG("getTagDetails for tag: " << tag);
  ATH_MSG_WARNING("SqliteReadSvc::getTagDetails is a dummy method");
  return;
}
