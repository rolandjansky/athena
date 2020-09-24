/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CORACOOL_CORACOOLDATABASE_H
#define CORACOOL_CORACOOLDATABASE_H
// CoraCoolDatabase.h - interface to a CoraCool database instance
// for managing referenced payload data associated to a COOL database instance
// Richard Hawkings, started 10/06

#include<string>
#include "RelationalAccess/IConnectionService.h"
#include "CoralBase/MessageStream.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IRecordSpecification.h"
#include "CoralBase/AttributeListSpecification.h"
#include "CoraCool/CoraCoolTypes.h"

namespace coral {
  class ISessionProxy;
  class ITable;
}

class CoraCoolDatabase {
 public:
  // instantiate a new CoraCoolDatabase instance
  // this should not normally be used - use openDatabase in CoraCoolDatabaseSvc
  CoraCoolDatabase(const std::string& m_dbconn, cool::IDatabasePtr cooldb,
	       coral::IConnectionService& coralsvc, const bool readonly=false);

  // connect to the underlying database, return true if connected
  // users should not normally need to call this directly
  bool connect();
  // disconnect from the underlying database, returns false if disconnected
  // users should not normally need to call this directly
  bool disconnect();

  // return the COOL database name associated with the folder
  const std::string dbname() const;

  // return the COOL database connection associated with the folder
  cool::IDatabasePtr coolDatabase() const;

  // create a CoraCool folder by creating the corresponding COOL folder and
  // the CoraCool table with the given name (with COOL dbinstance prepended)
  // the CoraCool metadata is added to the given descrpition string
  // throws CoraCool and Cool exceptions in case of problems
  CoraCoolFolderPtr createFolder(const std::string& coolpath,
     const std::string& coraltable,
     const cool::IRecordSpecification& fkspec,
     const cool::IRecordSpecification& payloadspec,
     const std::string& coralfk,
     const std::string& coralpk,
     const std::string& description="",
     const cool::FolderVersioning::Mode mode=cool::FolderVersioning::SINGLE_VERSION,
     const bool createParents=false);

  // get a pointer to the CoraCoolFolder object
  // throws CoraCool or Cool exceptions if folder does not exist, problems
  // with <coracool> metadata etc
  CoraCoolFolderPtr getFolder(const std::string& coolfolder);

  // check if the folder exists as a CoraCool folder
  // true if the corresponding COOL folder exists and there is the 
  // <coracool> meta data in the COOL description string
  bool existsFolder(const std::string& coolfolder);

  // parse the COOL folder description to extract the CORAL table name,
  // COOL and CORAL foreign and primary key names
  // returns true if all information extracted, false if syntax problems
  bool parseFolderDescription(const std::string& folderdesc,
			      std::string& tablename,
			      std::string& keycolcool,
			      std::string& fkeycolcoral,
			      std::string& pkeycolcoral);

  // delete the COOL folder and any associated CORAL table
  // returns true if folder successfully dropped
  // throws Cool or CoraCool exceptions if problems
  bool deleteFolder(const std::string& coolfolder);

 private:
  bool extractCoralConStr(const std::string& coolstr);
  std::string encodeAttrSpec(const cool::IRecordSpecification& spec);
  bool storeSpec(const std::string& tablename,const std::string& spec);

  cool::IDatabasePtr m_cooldb; // pointer to COOL database instance
  std::string m_dbconn; // CORAL database connection string
  std::string m_dbname; // COOL database instance name
  bool m_connected; // true when connection is active
  bool m_readonly; // true for readonly instance (can share connection)
  coral::IConnectionService& m_coralsvc; // CORAL connection service to use
  coral::ISessionProxy* m_proxy;
  coral::MessageStream m_log;
};

inline const std::string CoraCoolDatabase::dbname() const { return m_dbname; }

inline cool::IDatabasePtr CoraCoolDatabase::coolDatabase() const 
{return m_cooldb; }

#endif // CORACOOL_CORACOOLDATABASE_H
