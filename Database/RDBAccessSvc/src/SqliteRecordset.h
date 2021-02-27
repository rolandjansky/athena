/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SqliteRecordset.h
 *
 * @brief Declaration of the SqliteRecordset class
 *
 */

#ifndef RDBACCESSSVC_SQLITERECORDSET_H
#define RDBACCESSSVC_SQLITERECORDSET_H

#include "RDBAccessSvc/IRDBRecordset.h"
#include "AthenaBaseComps/AthMessaging.h"
#include "SqliteRecord.h"
#include <string>

class sqlite3;

/**
 * @class SqliteRecordset
 *
 * @brief SqliteRecordset implements IRDBRecordset interface.
 *        It is a container of records read from an SQLite table.
 *
 */

class SqliteRecordset final : public IRDBRecordset, public AthMessaging
{
 public:
  /// Construct empty recordset
  SqliteRecordset();
  
  /// Destructor, deletes all records
  ~SqliteRecordset() override;

  /// Constructs SQL query and retrieves data from the DB
  /// @param taginfo [IN] object holding information about the node tag
  void getData(sqlite3* db, const std::string& nodeName);

  /// @return number of records
  unsigned int size() const override;

  /// @return node name
  std::string nodeName() const override;

  /// @return tag name
  std::string tagName() const override {return std::string();}

  /// @param index [IN] index of the record
  /// @return RDBRecord by index
  const IRDBRecord* operator[](unsigned int index) const override;

  /// @return begin iterator
  IRDBRecordset::const_iterator begin() const override;

  /// @return end iterator
  IRDBRecordset::const_iterator end() const override;

 private:
  std::string       m_nodeName;
  SqliteInpDef_ptr  m_def;
  RecordsVector     m_records;
};

#endif
