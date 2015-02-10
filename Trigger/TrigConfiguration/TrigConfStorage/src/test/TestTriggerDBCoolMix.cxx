/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <string>

#include "TrigConfStorage/StorageMgr.h"
#include "TrigConfStorage/TrigConfCoolWriter.h"
#include "CoolApplication/DatabaseSvcFactory.h"
#include "RelationalAccess/ConnectionService.h"
#include "RelationalAccess/IConnectionServiceConfiguration.h"

#include "RelationalAccess/ISessionProxy.h"

#include "../../src/ReplicaSorter.h"


using namespace std;
using namespace TrigConf;


int main() {

   {

      unique_ptr<StorageMgr> tmpSM(new StorageMgr("TRIGGERDB", "", "", cout));

      tmpSM->sessionMgr().createSession();

   }


   bool readonly = true;

   cool::IDatabaseSvc& dbSvc = cool::DatabaseSvcFactory::databaseService();

   cool::IDatabasePtr dbPtr = dbSvc.openDatabase( "COOLONL_TRIGGER/CONDBR2", readonly );

}

