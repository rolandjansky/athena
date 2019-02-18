/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IOVDbConn.cxx - IOVDbSvc helper class to manage connections
// Richard Hawkings, started 24/11/08

#include "GaudiKernel/MsgStream.h"

#include "CoolApplication/DatabaseSvcFactory.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoraCool/CoraCoolDatabase.h"
#include "RelationalAccess/ConnectionService.h"

#include "IOVDbConn.h"

IOVDbConn::IOVDbConn(const std::string& dbname, const bool readOnly, 
                     MsgStream & msg) :
  m_log(msg),
  m_active(false),
  m_readonly(readOnly),
  m_abort(false),
  m_nconn(0),
  m_nfolder(0),
  m_connstr(dbname){
   //nop
}

IOVDbConn::~IOVDbConn() {}

cool::IDatabasePtr IOVDbConn::getCoolDb() {
  // only open if not already activated
  if (m_coolDb.get()==0) {
    // check connection not already aborted
    if (m_abort) {
      m_log << MSG::ERROR << "COOL connection for " << m_connstr << 
        " already aborted as invalid" << endmsg;
      // in this case, return the null connection ptr immediately to avoid
      // another full attempt to connect
      return m_coolDb;
    }
    // open new connection
    m_log << MSG::INFO << "Opening COOL connection for " << m_connstr << 
      endmsg;
    ++m_nconn;
    cool::IDatabaseSvc& dbSvc=cool::DatabaseSvcFactory::databaseService();
    try {
      m_coolDb=dbSvc.openDatabase(m_connstr,m_readonly);
      m_active=true;
    }
    catch (std::exception& e) {
      // create a new COOL conditions DB
      m_log << MSG::INFO << "*** COOL  exception caught: " << e.what() 
             << endmsg;
      m_log << MSG::INFO << "Create a new conditions database: " << m_connstr
             << endmsg;
      try {
        m_coolDb=dbSvc.createDatabase(m_connstr);
        m_active=true;
      }
      catch (std::exception&e ) {
        m_log << MSG::ERROR << "*** COOL  exception caught: " << e.what() 
               << endmsg;
        m_log << MSG::ERROR << 
          "Could not create a new conditions database - abort connection"
               << endmsg;
        m_abort=true;
        m_coolDb.reset();
      }
    }
  }
  return m_coolDb;
}

CoraCoolDatabasePtr IOVDbConn::getCoraCoolDb() {
  // only open if not already activated
  if (m_coracoolDb.get()==0) {
    // open new connection
    m_log << MSG::INFO << "Opening CoraCool connection for " << m_connstr << 
      endmsg;
    coral::ConnectionService connSvc;
    m_coracoolDb=CoraCoolDatabasePtr(new CoraCoolDatabase(m_connstr,
                                                          m_coolDb,connSvc,m_readonly));
    m_coracoolDb->connect();
  }
  return m_coracoolDb;
}

void IOVDbConn::setInactive() {
  if (m_coolDb.get()!=0) {
    m_log << MSG::INFO << "Disconnecting from " << m_connstr << endmsg;
    try {
      m_coolDb->closeDatabase();
    }
    catch (std::exception& e) {
      m_log << MSG::INFO << "Exception caught when disconnecting: " <<
        e.what() << endmsg;
    }
    m_coolDb.reset();
  }
  if (m_coracoolDb.get()!=0) {
    m_log << MSG::INFO << "Disconnecting CoraCool from " << m_connstr << 
      endmsg;
    try { 
      m_coracoolDb->disconnect();
    }
    catch (std::exception& e) {
      m_log << MSG::INFO << "Exception caught when disconnecting CoraCool: " 
             << e.what() << endmsg;
    }
    m_coracoolDb.reset();
  }
  m_active=false;
}

void IOVDbConn::setReadOnly(const bool readOnly) {
  // reset readonly state, assuming different from current one
  // close current connection if required
  setInactive();
  m_readonly=readOnly;
}

void IOVDbConn::summary(const float fread) {
  // print summary of usage
  m_log << MSG::INFO << "Connection " << m_connstr << " : nConnect: " <<
    m_nconn << " nFolders: " << m_nfolder <<  " ReadTime: (( " << std::fixed <<
    std::setw(8) << std::setprecision(2) << fread << " ))s" << endmsg;
}
