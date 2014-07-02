/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CORACOOL_CORACOOLDATABASESVC_H
#define CORACOOL_CORACOOLDATABASESVC_H
// CoraCoolDatabaseSvc.h
// a service to create CoraCoolDatabase objects given a SEAL context (where
// CORAL libraries are already loaded) the corresponding COOL IDatabaseSvc 
// (plays an analogous role to COOL IDatabaseSvc)
// This can be created directly, or via CoraCoolDatabaseSvcFactory singleton
// which sets up CoraCool and COOL standalone in one call
//
// Richard Hawkings, started 27/10/06

#include<string>
#include "CoolKernel/IDatabase.h"
#include "RelationalAccess/IConnectionService.h"
#include "CoraCool/CoraCoolTypes.h"

namespace cool {
  class IDatabaseSvc;
}

class CoraCoolDatabaseSvc {
 public:
  // create a CoraCoolDatabaseSvc given CORAL connection svc and COOL service
  CoraCoolDatabaseSvc(coral::IConnectionService& coralsvc, 
		      cool::IDatabaseSvc& cooldb);

  // return a reference to the associated COOL database service
  cool::IDatabaseSvc& coolIDatabaseSvc();

  // open CoraCool database, given external COOL reference
  CoraCoolDatabasePtr openDatabase(const std::string& dbconn,
		 cool::IDatabasePtr cooldb,bool readonly=false);
  // open CoraCool database, without external COOL reference
  CoraCoolDatabasePtr openDatabase(const std::string& dbconn,
		 bool readonly=false);

 private:
  coral::IConnectionService& m_coralsvc;
  cool::IDatabaseSvc& m_cooldb;
};

#endif // CORACOOL_CORACOOLDATABASESVC_H
