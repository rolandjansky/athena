/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SqliteRecordset.cxx
 *
 * @brief Implementation of the SqliteRecordset class
 *
 */

#include "SqliteRecordset.h"
#include "AthenaKernel/getMessageSvc.h"

#include <sqlite3.h>
#include <stdexcept>
#include <sstream>
#include <set>

SqliteRecordset::SqliteRecordset()
  : AthMessaging(Athena::getMessageSvc(), "SqliteRecordset")
  , m_nodeName("")
  , m_def(std::make_shared<SqliteInpDef>())
{
}

SqliteRecordset::~SqliteRecordset()
{
  for(auto record : m_records) {
    delete record;
  }
}

void SqliteRecordset::getData(sqlite3* db, const std::string& nodeName)
{
  ATH_MSG_DEBUG("getData for " << nodeName);
  m_nodeName = nodeName;

  std::ostringstream sql;
  sql << "select * from " << m_nodeName << " order by " << m_nodeName << "_data_id";
  sqlite3_stmt* st{nullptr};
  int rc = sqlite3_prepare_v2(db, sql.str().c_str(), -1, &st, NULL);
  int ctotal = sqlite3_column_count(st);

  bool all_ok{true};
  SqliteRecord* record{nullptr};

  while(true) {
    rc = sqlite3_step(st);
    if(rc == SQLITE_ROW) {
      record = new SqliteRecord(m_def);

      // Loop throug the fields of the retrieved record
      for(int i=0; i<ctotal; ++i) {

	// The feature of SQLite: if in the given record some fields have NULL values,
	// then the data type of the corresponding columns is reported as NULL.
	// This means that we need to be able to build the Def gradually, as we read
	// in new records of the table
	
	// Do we need to extend Def?
	std::string columnName = sqlite3_column_name(st,i);
	bool extendDef = (m_def->find(columnName)==m_def->end());

	auto columnType = sqlite3_column_type(st,i);
	SqliteInpType inpType{SQLITEINP_UNDEF};
	SqliteInp val;

	switch(columnType) {
	case SQLITE_INTEGER:
	  inpType = SQLITEINP_INT;
	  val = sqlite3_column_int(st,i);
	  break;
	case SQLITE_FLOAT:
	  inpType = SQLITEINP_DOUBLE;
	  val = sqlite3_column_double(st,i);
	  break;
	case SQLITE_TEXT:
	  inpType = SQLITEINP_STRING;
	  val = std::string((char*)(sqlite3_column_text(st,i)));
	  break;
	case SQLITE_NULL:
	  continue;
	default:
	  break;
	}

	if(inpType==SQLITEINP_UNDEF) {
	  all_ok = false;
	  ATH_MSG_ERROR("Unexpected data type in column " << columnName << " of the table " << m_nodeName);
	  break;
	}

	if(extendDef) {
	  (*m_def)[columnName] = inpType;
	}
	record->addValue(columnName,val);
      }
      m_records.push_back(record);
    }
    else if(rc == SQLITE_ERROR) {
      ATH_MSG_ERROR(sqlite3_errmsg(db));
      all_ok = false;
      break;
    }
    else if(rc == SQLITE_DONE) {
      break;
    }
    else {
      ATH_MSG_ERROR("Unexpected error occurred while fetching " << m_nodeName);
      all_ok = false;
      break;
    }
  }

  if(!all_ok) {
    // Do memory cleanup
    delete record;
    for(auto rec : m_records) {
      delete rec;
    }
    m_records.clear();
  }
}

unsigned int SqliteRecordset::size() const 
{
  return m_records.size();
}

std::string SqliteRecordset::nodeName() const
{
  return m_nodeName;
}

const IRDBRecord* SqliteRecordset::operator[](unsigned int index) const 
{
  return m_records[index];
}
IRDBRecordset::const_iterator SqliteRecordset::begin() const
{
  return m_records.begin();
}

IRDBRecordset::const_iterator SqliteRecordset::end() const
{
  return m_records.end();
}
