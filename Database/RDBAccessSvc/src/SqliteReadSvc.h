/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SqliteReadSvc.h
 *
 * @brief Declaration of SqliteReadSvc class
 *
 */

#ifndef RDBACCESSSVC_SQLITEREADSVC_H
#define RDBACCESSSVC_SQLITEREADSVC_H

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "SqliteRecordset.h"

#include "AthenaBaseComps/AthService.h"

#include <sqlite3.h>
#include <string>
#include <map>
#include <mutex>

class ISvcLocator;

template <class TYPE> class SvcFactory;

// Map of recordset pointers by table name
typedef std::map<std::string, IRDBRecordset_ptr> RecordsetPtrMap;

/**
 * @class SqliteReadSvc
 *
 * @brief SqliteReadSvc implementats IRDBAccessSvc interface for reading
 *        plain tables in the Geometry SQLite database
 *
 */

class SqliteReadSvc final : public AthService, virtual public IRDBAccessSvc 
{
 public:
  /// Standard Service Constructor
  SqliteReadSvc(const std::string& name, ISvcLocator* svc);

  StatusCode finalize() override;
  StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface ) override;

  friend class SvcFactory<SqliteReadSvc>;

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IRDBAccessSvc; }

  /// Open the SQLite database
  /// This method has no effect if the connection has already been opened
  /// @param connName [IN] path to the SQLite database file
  /// @return success/failure
  bool connect(const std::string& connName) override;

  /// Dummy overrider of the virtual function
  /// @return success/failure
  bool disconnect(const std::string&) override;

  /// Closes the database connection
  /// @return success/failure
  bool shutdown(const std::string&) override;

  /// Provides access to the Recordset object containing HVS-tagged data.
  /// @param node [IN] name of the table. Other input parameters are dummy
  /// @return pointer to the recordset object
  IRDBRecordset_ptr getRecordsetPtr(const std::string& node
				    , const std::string& 
				    , const std::string& 
				    , const std::string&) override;

  /// Dummy overrider of the virtual function
  /// @param childNode [IN] the name of the table
  /// @return the name of the table if exists, otherwise an empty string
  std::string getChildTag(const std::string& childNode
			  , const std::string& 
			  , const std::string& 
			  , const std::string& ) override;

  /// Dummy overrider of the virtual function (for now)
  std::unique_ptr<IRDBQuery> getQuery(const std::string& node
				      , const std::string& 
				      , const std::string& 
				      , const std::string& ) override;

  /// Dummy overrider of the virtual function
  void getTagDetails(RDBTagDetails& tagDetails
		     , const std::string& tag
		     , const std::string& ) override;

private:
  RecordsetPtrMap m_recordsets;
  sqlite3*        m_db{nullptr};
  std::mutex      m_recordsetMutex;
  std::mutex      m_sessionMutex;
};

#endif 
