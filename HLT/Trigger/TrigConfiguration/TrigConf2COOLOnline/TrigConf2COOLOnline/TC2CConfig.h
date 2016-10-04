/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONF_TC2CCONFIG
#define TRIGCONF_TC2CCONFIG

#include <string>
#include <iosfwd>

class TC2CConfig;

std::ostream& operator<<( std::ostream& os, const TC2CConfig& event );

class TC2CConfig {
     
   friend std::ostream& operator<<( std::ostream& os, const TC2CConfig& event );

public:
   TC2CConfig();
   ~TC2CConfig();

   std::string&     PartitionName()        { return fPartitionName; }
   std::string&     CoolDb()               { return fCoolDb; }
   std::string&     DBType()               { return fDBType; }
   std::string&     DBServer()             { return fDBServer; }
   std::string&     DBConnection()         { return fDBConnection; }
   std::string&     DBUser()               { return fDBUser; }
   std::string&     DBPasswd()             { return fDBPasswd; }
   std::string&     DBName()               { return fDBName; }
   unsigned long&   DBSMK()                { return fDBSMK; }
   unsigned long&   DBL1PSK()              { return fDBL1PSK; }
   unsigned long&   DBHLTPSK()             { return fDBHLTPSK; }
   std::string&     DBSMcomment()          { return fDBSMcomment; }
   std::string&     DBL1PScomment()        { return fDBL1PScomment; }
   std::string&     DBHLTPScomment()       { return fDBHLTPScomment; }
   bool&            UseTriggerDB()         { return fUseTriggerDB; }
   uint32_t&        CurrentRunNumber()     { return fCurrentRunNumber; }
   uint32_t&        LastWrittenRunNumber() { return fLastWrittenRunNumber; }
   bool&            COOLconnStat()         { return fCOOLconnStat; }
   bool&            TriggerDBconnStat()    { return fTriggerDBconnStat; }
   bool&            L1toIS()               { return fL1toIS; }
   bool&            L1toCOOL()             { return fL1toCOOL; }

   void             TriggerDbConnectionParams(std::string& conn, std::string& user, std::string& pw);
    
   bool&                MCKtoCOOL() { return fMCKtoCOOL; }
    
   unsigned long&       DBMCK() { return fDBMCK; }
   std::string&         DBMCKrelease() { return fDBMCKrelease; }
   std::string&         DBMCKinfo() { return fDBMCKinfo; }


private:
   std::string        fPartitionName {""};
   std::string        fCoolDb        {""};
   std::string        fDBType        {""};
   std::string        fDBServer      {""};
   std::string        fDBConnection  {""};
   std::string        fDBUser        {""};
   std::string        fDBPasswd      {""};
   std::string        fDBName        {""};
   unsigned long      fDBSMK    {0};
   unsigned long      fDBL1PSK  {0};
   unsigned long      fDBHLTPSK {0};
   std::string        fDBSMcomment    {""};
   std::string        fDBL1PScomment  {""};
   std::string        fDBHLTPScomment {""};
   bool               fUseTriggerDB {false};
   uint32_t           fCurrentRunNumber     {0};
   uint32_t           fLastWrittenRunNumber {0};
   bool               fCOOLconnStat {false};
   bool               fTriggerDBconnStat {false};
   bool               fL1toIS {false};
   bool               fL1toCOOL {false};
    
    bool                fMCKtoCOOL {false};
    
    unsigned long       fDBMCK {0};
    std::string         fDBMCKrelease {""};
    std::string         fDBMCKinfo {""};

};
#endif																																											
