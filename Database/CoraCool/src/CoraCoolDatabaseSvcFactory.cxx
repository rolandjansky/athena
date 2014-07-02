/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CoraCoolDatabaseSvcFactory.cxx
// implementation of CoraCoolDatabaseSvcFactory
// Richard Hawkings, started 27/10/06

#include "CoolApplication/Application.h"
#include "CoraCool/CoraCoolDatabaseSvc.h"
#include "CoraCool/CoraCoolDatabaseSvcFactory.h"

CoraCoolDatabaseSvc& CoraCoolDatabaseSvcFactory::databaseService() {
  static cool::Application app;
  static CoraCoolDatabaseSvc coradbsvc(app.connectionSvc(),
				       app.databaseService());
  return coradbsvc;
}
