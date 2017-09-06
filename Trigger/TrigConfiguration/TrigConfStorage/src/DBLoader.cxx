/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./DBHelper.h"
#include "TrigConfStorage/DBLoader.h"

#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/SchemaException.h"

#include <CoralBase/Attribute.h>
#include <CoralBase/AttributeList.h>

#include <stdexcept>

using namespace std;
using namespace TrigConf;

unsigned int TrigConf::DBLoader::s_triggerDBSchemaVersion = 0;
unsigned int TrigConf::DBLoader::s_run = 0;
unsigned int TrigConf::DBLoader::s_ctpVersion = 0;
unsigned int TrigConf::DBLoader::s_l1Version = 0;

DBLoader::DBLoader( const std::string& name, StorageMgr& sm, coral::ISessionProxy& session ) : 
   TrigConfMessaging(name),
   m_verbose(1),
   m_storageMgr( sm ),
   m_session( session ) , 
   m_sessionOwner(false)
{}


DBLoader::DBLoader( StorageMgr& sm, coral::ISessionProxy& session ) : 
   DBLoader("DBLoader", sm, session)
{}


void TrigConf::DBLoader::startSession()
{
   if ( ! m_session.transaction().isActive() ) {
      //std::cout << "DBLoader: startSession(readonly=true)" << std::endl;
      bool readOnly = true;
      m_session.transaction().start(readOnly);
      m_sessionOwner = true;
   }
}

void TrigConf::DBLoader::commitSession() const
{
   if ( m_session.transaction().isActive() && m_sessionOwner) {
      m_session.transaction().commit();
   }
}


bool
DBLoader::isRun2() {
   if(s_run==0)
      loadSchemaVersion();
   return s_run == 2;
}

void
DBLoader::setLevel(MSGTC::Level lvl) {
   msg().setLevel(lvl);

   switch(lvl) {
   case MSGTC::ALWAYS: m_verbose = 5; break;
   case MSGTC::VERBOSE: m_verbose = 4; break;
   case MSGTC::DEBUG: m_verbose = 3; break;
   case MSGTC::INFO: m_verbose = 2; break;
   case MSGTC::WARNING:
   case MSGTC::ERROR:
   case MSGTC::FATAL: m_verbose = 0; break;
   default: m_verbose = 0;
   }
}


void
DBLoader::loadSchemaVersion() const
{
   bool mySession = false;
   if ( ! m_session.transaction().isActive() ) {
      m_session.transaction().start(true);
      mySession = true;
   }

   std::unique_ptr< coral::IQuery > q( m_session.nominalSchema().tableHandle( "TRIGGER_SCHEMA").newQuery() );
   q->setRowCacheSize( 1 );

   //Output data and types
   coral::AttributeList attList;
   attList.extend<int>( "TS_ID" );
   q->defineOutput(attList);
   q->addToOutputList( "TS_ID" );

   q->addToOrderList("TS_ID desc");
   coral::ICursor& cursor = q->execute();

   if ( ! cursor.next() ) {
      TRG_MSG_ERROR("Table TRIGGER_SCHEMA is not filled");
      if ( mySession ) m_session.transaction().commit();
      throw std::runtime_error( "DBLoader::loadSchemaVersion() >> Table TRIGGER_SCHEMA is not filled" );
   }

   const coral::AttributeList& row = cursor.currentRow();
   s_triggerDBSchemaVersion = row["TS_ID"].data<int>();

   TRG_MSG_INFO("TriggerDB schema version: " << s_triggerDBSchemaVersion);

   s_run = m_session.nominalSchema().existsTable( "ACTIVE_MASTERS" ) ? 2 : 1;

   TRG_MSG_INFO("Database has Run " << s_run << " schema");
   TRG_MSG_INFO("Total number of tables : " <<  m_session.nominalSchema().listTables().size());

   //commitSession();
   if ( mySession ) m_session.transaction().commit();
}

bool
TrigConf::DBLoader::loadL1MasterKey(int smk, int& Lvl1MasterKey) {
   try {
      startSession();
      
      unique_ptr< coral::IQuery > q( m_session.nominalSchema().tableHandle( "SUPER_MASTER_TABLE").newQuery() );
      q->setRowCacheSize( 5 );

      //Bind list
      coral::AttributeList bindings;
      bindings.extend<int>("smtid");
      bindings[0].data<int>() = smk;
      q->setCondition( "SMT_ID = :smtid", bindings );

      //Output data and types
      coral::AttributeList attList;
      attList.extend<int>( "SMT_L1_MASTER_TABLE_ID" );
      fillQuery(q.get(), attList);

      coral::ICursor& cursor = q->execute();
      if ( ! cursor.next() ) {
         msg() << "DBLoader:           No such SuperMaster key exists " << smk << endl;
         throw runtime_error( "DBLoader:        SuperMasterKey not available" );
      }
	
      const coral::AttributeList& row = cursor.currentRow();
      Lvl1MasterKey = row["SMT_L1_MASTER_TABLE_ID"].data<int>();
   }
   catch( const std::exception& e ) {
      commitSession();
      msg() << "DBLoader:         C++ exception: " << e.what() << std::endl;
      throw;
   }
   commitSession();
   return true; 
}



bool
TrigConf::DBLoader::loadL1MenuKey(int SuperMasterKey, int& Lvl1MenuKey) {
   try {

      int l1Master = 0;
      loadL1MasterKey(SuperMasterKey, l1Master);

      startSession();
	
      unique_ptr< coral::IQuery > q( m_session.nominalSchema().tableHandle( "L1_MASTER_TABLE").newQuery() );
      q->setRowCacheSize( 5 );

      //Bind list
      coral::AttributeList bindings;
      bindings.extend<int>("l1mtid");
      bindings[0].data<int>() = l1Master;
      q->setCondition( "L1MT_ID = :l1mtid" , bindings );

      //Output data and types
      coral::AttributeList attList;
      attList.extend<int>( "L1MT_TRIGGER_MENU_ID" );
      q->defineOutput(attList);
      q->addToOutputList( "L1MT_TRIGGER_MENU_ID" );

      coral::ICursor& cursor = q->execute();
      if ( ! cursor.next() ) {
         msg() << "DBLoader >> No such L1 Master key exists " << l1Master << std::endl;
         throw std::runtime_error( "DBLoader >> L1MasterKey not available" );
         commitSession();
      }
	
      const coral::AttributeList& row = cursor.currentRow();
      Lvl1MenuKey = row["L1MT_TRIGGER_MENU_ID"].data<int>();

      commitSession();

   }
   catch( const std::exception& e ) {
      msg() << "DBLoader >> Standard C++ exception: " << e.what() << std::endl;
      m_session.transaction().rollback();
      throw;
   }
   return true; 
}

// static member definition
TrigConf::DBLoader::ENV TrigConf::DBLoader::m_env = TrigConf::DBLoader::CTP;
