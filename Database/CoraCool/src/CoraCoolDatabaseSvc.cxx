/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CoraCoolDatabaseSvc.cxx
// implementation for CoraCoolDatabaseSvc
// Richard Hawkings, started 27/10/06

#include "CoraCool/CoraCoolDatabase.h"
#include "CoraCool/CoraCoolDatabaseSvc.h"
#include "CoraCool/CoraCoolException.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "RelationalAccess/IConnectionService.h"

CoraCoolDatabaseSvc::CoraCoolDatabaseSvc(coral::IConnectionService& coralsvc, 
                                         cool::IDatabaseSvc& cooldb)
        :
        m_coralsvc(coralsvc), m_cooldb(cooldb) {}

cool::IDatabaseSvc& CoraCoolDatabaseSvc::coolIDatabaseSvc() {
  return m_cooldb; }

CoraCoolDatabasePtr CoraCoolDatabaseSvc::openDatabase(
 const std::string& dbconn,cool::IDatabasePtr cooldb, bool readonly) {
  CoraCoolDatabasePtr dbase(new CoraCoolDatabase(dbconn,
                                                 cooldb,m_coralsvc,readonly));
  return dbase;
}

CoraCoolDatabasePtr CoraCoolDatabaseSvc::openDatabase(
 const std::string& dbconn,bool readonly) {
  cool::IDatabasePtr cooldb;
  try {
    cooldb=m_cooldb.openDatabase(dbconn,readonly);
  }
  catch (cool::Exception& e) {
    // try to create database - assuming it did not exist before
    try {
      if (readonly) throw std::exception();
      cooldb=m_cooldb.createDatabase(dbconn);
    }
    catch (std::exception& e) {
      throw CoraCoolException("Cannot connect to or create COOL database",
			    "CoraCoolDatabaseSvc::openDatabase");
    }
  }
  CoraCoolDatabasePtr dbase(new CoraCoolDatabase(dbconn,
                                                 cooldb,m_coralsvc,readonly));
  return dbase;
}
