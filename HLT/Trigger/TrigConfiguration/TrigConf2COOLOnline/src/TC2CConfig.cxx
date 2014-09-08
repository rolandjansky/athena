/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <algorithm>

#include "TrigConf2COOLOnline/TC2CConfig.h"

using std::endl;

TC2CConfig::TC2CConfig()
{}

TC2CConfig::~TC2CConfig()
{}

void
TC2CConfig::TriggerDbConnectionParams(std::string& conn, std::string& user, std::string& pw) {
  user = DBUser();
  pw = DBPasswd();

  std::string dbtype = DBType();
  std::transform(dbtype.begin(),dbtype.end(),dbtype.begin(),tolower);
  if (dbtype == "oracle") {
    conn = "oracle://" + DBServer() + "/" + DBName();
  } else if (dbtype == "mysql") {
    conn = "mysql://" + DBServer() + "/" + DBName();
  } else if (dbtype == "sqlite") {
    conn = "sqlite://" + DBServer() + "/" + DBName();
  } else if (dbtype == "dblookup") {
    conn = DBServer();
    user = pw = "";
  }
}


std::ostream& operator<<(std::ostream& os, const TC2CConfig& c) {
  os << "Job configuration:" << endl
     << "  COOL connection : " << c.fCoolDb << endl
     << "  Use TriggerDB   : " << (c.fUseTriggerDB?"true":"false") << endl;
  if(c.fUseTriggerDB)
    os << "    Trigger DB connection parameter" << endl
       << "      Type   : " << c.fDBType << endl
       << "      Server : " << c.fDBServer << endl
       << "      User   : " << c.fDBUser << endl
       << "      Passwd : " << c.fDBPasswd << endl
       << "      Name   : " << c.fDBName << endl
       << "    SMK             : " << c.fDBSMK << endl
       << "    L1 PSK          : " << c.fDBL1PSK << endl
       << "    HLT PSK         : " << c.fDBHLTPSK << endl;
  os << "  Current run number: " << c.fCurrentRunNumber << endl;
  os << "  Last run number written to COOL: " << c.fLastWrittenRunNumber << endl;
  os << "  COOL connection status: " << (c.fCOOLconnStat?"connected":"disconnected") << endl;
  os << "  TriggerDB connection status: " << (c.fTriggerDBconnStat?"connected":"disconnected") << endl;
  
  return os;
}
