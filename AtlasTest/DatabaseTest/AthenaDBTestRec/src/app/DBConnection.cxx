/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DBConnection.cxx
// Class to hold details of a COOL DB connection
// Richard Hawkings, started 20/1/06

#include <iostream>
#include "CoolKernel/IDatabaseSvc.h"
#include "CoraCool/CoraCoolDatabaseSvcFactory.h"
#include "RelationalAccess/ConnectionService.h"
#include "DBConnection.h"

DBConnection::DBConnection(const std::string tech, const std::string server,
	       const std::string schema, const std::string dbname,
			   const std::string user, const std::string passwd) :
  m_tech(tech), m_server(server), m_schema(schema), m_dbname(dbname),
  m_user(user), m_passwd(passwd), m_active(false), m_coracool(false)
{}

DBConnection::DBConnection(const std::string options, 
			   const std::string newschema) 
{
  // split the input string into fields separated by :
  std::vector<std::string> split;
  unsigned int iofs1=0;
  while (iofs1<options.size()) {
    std::string::size_type iofs2=options.find(":",iofs1);
    if (iofs2==std::string::npos) iofs2=options.size();
    split.push_back(options.substr(iofs1,iofs2-iofs1));
    iofs1=iofs2+1;
  }
  // override default schema if needed
  if (split.size()>3) {
    m_tech=split[0];
    m_server=split[1];
    m_schema=split[2];
    m_dbname=split[3];
    if (split.size()>4) m_user=split[4];
    if (split.size()>4) m_passwd=split[5];
  }
  if (newschema!="") m_schema=newschema;
  m_active=false;
  m_coracool=false;
}

DBConnection::~DBConnection() {}
 
std::string DBConnection::coolStr() const {
  std::string coolcon;
  if (m_tech=="logical") {
    coolcon=m_schema+"/"+m_dbname;
  } else {
    coolcon=m_tech+"://"+m_server+";schema="+m_schema;
    if (m_user!="") coolcon+=";user="+m_user;
    coolcon+=";dbname="+m_dbname;
    if (m_passwd!="") coolcon+=";password="+m_passwd;
  }
  return coolcon;
}

std::string DBConnection::iovdbStr() const {
  return "<db>"+coolStr()+"</db>";
}

cool::IDatabasePtr DBConnection::open() {
  // only open if not already activated
  if (m_coolptr.get()==0) {
    // open the COOL DB 
    m_coracool=false;
    const std::string coolstr=coolStr();
    CoraCoolDatabaseSvc& corasvc=CoraCoolDatabaseSvcFactory::databaseService();
    cool::IDatabaseSvc& dbSvc=corasvc.coolIDatabaseSvc();
    std::cout << "Attempt to open COOL database with connection string: "
	      << coolstr << std::endl;
    try {
      m_coolptr=dbSvc.openDatabase(coolstr,false);
      std::cout << "Connected to existing database" << std::endl;
      m_active=true;
    }
    catch (std::exception& e) {
      std::cout << "COOL exception caught: " << e.what() << std::endl;
      std::cout << "Try to create new conditions DB" << std::endl;
      try {
        m_coolptr=dbSvc.createDatabase(coolstr);
        std::cout << "Database creation succeeded" << std::endl;
        m_active=true;
      }
      catch (std::exception& e) {
        std::cout << "Database creation failed" << std::endl;
        m_active=false;
      }
    }
  }
  return m_coolptr;
}

CoraCoolDatabasePtr DBConnection::coradbptr() {
  // only open if not already activated
  if (m_coradb.get()==0) {
    const std::string coolstr=coolStr();
    CoraCoolDatabaseSvc& corasvc=CoraCoolDatabaseSvcFactory::databaseService();
    std::cout << "Attempt to open CoraCool database for " << coolstr <<
      std::endl;
    try {
      m_coradb=corasvc.openDatabase(coolstr,false);
      m_coracool=true;
    }
    catch (std::exception& e) {
      std::cout << "CoraCool exception caught: " << e.what() << std::endl;
    }
  }
  return m_coradb;
}

void DBConnection::close() {
  if (m_active) {
    std::cout << "Closing DB connection " << m_schema << " coracool " << 
      m_coracool << std::endl;
    if (m_coracool) {
      m_coradb->disconnect();
      m_coradb.reset();
      m_coracool=false;
    }
    m_coolptr->closeDatabase();
    m_coolptr.reset();
    m_active=false;
  }
}

