/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALIBDBCONNECTION_H
#define CALIBDBCONNECTION_H

//c - c++
#include "string"
#include "set"

namespace coral {
  class IRelationalDomain;
  class ISessionProxy;
  class Context;
  class IQuery;
  class ITableDataEditor;
}//namespace coral

namespace MuonCalib{

class CalibDbConnection {
 public:
//==================constructor - destructor====================================
  CalibDbConnection(const std::string& ConnectionString, const std::string& WorkingSchema );
  virtual ~CalibDbConnection();
//==================public member functions=====================================
  //connection control
  bool OpenConnection();
  inline void SetLogin(const std::string & username, const std::string &password) {
    m_username = username;
    m_password = password;
  }
  inline void GetLogin(std::string & username, std::string &password) const {
    username = m_username;
    password = m_password;
  }
  inline const std::string GetConnectionString() const {
    return m_connection_string;
  }
  //set target user - the given login will be used as proxy
  inline void SetTargetUser(const std::string & target_user) {
    m_target_user = target_user;
  }
  //transaction control
  void OpenTransaction();
  void Commit();
  void Rollback();
  //get table editor - for insert and update ueries
  coral::ITableDataEditor& GetTableEditor(const std::string & table_name);
  //get query
  coral::IQuery* GetQuery();
  void DestroyQuery(coral::IQuery* query);
  //get latest header id
 private:
//==================private data members========================================
  //connection strings
  std::string m_connection_string, m_working_schema;
  bool m_comp_loaded;
  //pointer to the open query;
  std::set<coral::IQuery*> m_queries;
  // The application context
  coral::Context* m_context;
  // session proxy
  coral::ISessionProxy *m_session;
  // username 
  std::string m_username;
  // password 
  std::string m_password;
  //target user
  std::string m_target_user;
  //is true if transaction is open
  bool m_transaction;
//================private member functions=====================================
  // Returns a reference to the domain object for a given connection
  coral::IRelationalDomain& domain( const std::string& connectionString );
};

} //namespace MuonCalib

#endif
