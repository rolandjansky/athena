/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENADBTESTREC_DBCONNECTION_H
#define ATHENADBTESTREC_DBCONNECTION_H
// DBConnection.h
// Class to hold details of a COOL DB connection
// Richard Hawkings, started 19/1/06

#include <string>

#include "CoolKernel/IDatabaseSvc.h"
#include "CoraCool/CoraCoolDatabase.h"

class DBConnection {
 public:
  DBConnection(const std::string tech, const std::string server,
	       const std::string schema, const std::string dbname,
	       const std::string user, const std::string passwd);
  DBConnection(const std::string options, const std::string newschema="");

  ~DBConnection();

  cool::IDatabasePtr open();
  void close();
  bool isActive() const;
  std::string coolStr() const;
  std::string iovdbStr() const;
  CoraCoolDatabasePtr coradbptr();

 private:
  std::string m_tech;
  std::string m_server;
  std::string m_schema;
  std::string m_dbname;
  std::string m_user;
  std::string m_passwd;
  bool m_active;
  bool m_coracool;
  cool::IDatabasePtr m_coolptr;
  CoraCoolDatabasePtr m_coradb;
};

inline bool DBConnection::isActive() const {return m_active;}

#endif // ATHENADBTESTREC_DBCONNECTION_H
