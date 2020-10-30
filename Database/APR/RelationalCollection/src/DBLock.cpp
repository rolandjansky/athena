/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "DBLock.h"

#include "POOLCore/Exception.h"

#include "RelationalCollectionNames.h"
#include "RelationalCollectionBindVariables.h"

#include "RelationalAccess/TableDescription.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ITablePrivilegeManager.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITableDataEditor.h"

#include "CoralBase/MessageStream.h"
#include "CoralBase/AttributeSpecification.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/TimeStamp.h"


#ifdef WIN32
#   include <windows.h>
#   define sleep(x) Sleep((x)*1000)  
std::string userinfo() {
   return "Windows";
}
#elif defined __APPLE__
std::string userinfo() {
   return "Apple";
}
#else
// hopefully UNIX
#   include <unistd.h>
#   include <limits.h>
#   include <pwd.h>
std::string userinfo() {
   static char    hostname[HOST_NAME_MAX+1];
   if( gethostname( hostname, HOST_NAME_MAX ) ) {
      *hostname = 0;  //error
   }
   hostname[HOST_NAME_MAX] = 0;
   struct passwd *pwdent = getpwuid(geteuid());
   const char *username_p = (pwdent? pwdent->pw_name : 0);
   std::string username;
   if( username_p )
      username = username_p; 
   return std::string(hostname) + "/" + username;
}

#endif


using namespace pool;
using namespace std;
using namespace pool::RelationalCollection;


 
DBLock::DBLock( coral::ISessionProxy& session, const std::string& collName )
      : m_session( session ),
        m_name( collName ),
        m_locked( false ),
        m_autorelease( true ),
        m_lockIngoreAge( 4000 ),
        m_maxWait( 300 )
{
   const char*  env = getenv("POOL_RCOLL_LOCKAGELIMIT");
   if( env ) {
      int age = atoi(env);
      if( age < 60 && age != 0 ) {
         // 0 means infinity
         m_lockIngoreAge = 60;
      } else {
         m_lockIngoreAge = age;
      }
   }
   env = getenv("POOL_RCOLL_LOCKTIMEOUT");
   if( env ) {
      int to = atoi(env);
      if( to < 10 && to != 0 ) {
         // 0 means infinity
         m_maxWait = 10;
      } else {
         m_maxWait = to;
      }
   }
}
 
 
DBLock::~DBLock()
{
   if( m_locked && m_autorelease ) {
      unlock();
      m_session.transaction().commit();
   }
}


void
DBLock::setAutorelease( bool ar )
{
   m_autorelease = ar;
}


void
DBLock::lock( bool XTrans )
{
   using namespace pool::RelationalCollection;
   coral::MessageStream log( "pool::RelationalCollection::DBLock" );
    
   coral::ISchema& nominalSchema = m_session.nominalSchema();
   if( !nominalSchema.existsTable( RelationalCollectionNames::nameOfCollectionLockTable() ) ) {
      // create the table where the locking is done
      coral::TableDescription description( RelationalCollectionNames::nameOfCollectionLockTable() );
      description.setName( RelationalCollectionNames::nameOfCollectionLockTable() );
      description.insertColumn( 
         RelationalCollectionNames::entryIDVarInCollectionLockTable(),
         coral::AttributeSpecification::typeNameForType<std::string>(), 50, false );
      description.insertColumn( 
         RelationalCollectionNames::collectionNameVarInCollectionLockTable(),
         coral::AttributeSpecification::typeNameForType<std::string>(), 500, false );
      description.insertColumn( 
         RelationalCollectionNames::clientInfoVarInCollectionLockTable(),
         coral::AttributeSpecification::typeNameForType<std::string>(), 500, false );
      description.insertColumn( 
         RelationalCollectionNames::lockTypeVarInCollectionLockTable(),
         coral::AttributeSpecification::typeNameForType<std::string>(), 20, false );
      description.insertColumn( 
         RelationalCollectionNames::timestampVarInCollectionLockTable(),
         coral::AttributeSpecification::typeNameForType<coral::TimeStamp>(), 2, false );
 
      description.setPrimaryKey( RelationalCollectionNames::entryIDVarInCollectionLockTable() );
      log << coral::Debug << "  Creating lock TABLE:" << RelationalCollectionNames::nameOfCollectionLockTable() <<  coral::MessageStream::endmsg;
      nominalSchema.createTable( description ); //.privilegeManager().grantToPublic( coral::ITablePrivilegeManager::Select );
   }

   int  total_wait = 0;
   do { 
      coral::ITable& lockTable = m_session.nominalSchema().tableHandle(
         RelationalCollectionNames::nameOfCollectionLockTable() );
      coral::IQuery* query = lockTable.newQuery();
      query->addToOutputList( RelationalCollectionNames::entryIDVarInCollectionLockTable() );
      query->addToOutputList( RelationalCollectionNames::collectionNameVarInCollectionLockTable() );
      query->addToOutputList( RelationalCollectionNames::lockTypeVarInCollectionLockTable() );
      query->addToOutputList( RelationalCollectionNames::timestampVarInCollectionLockTable() );
      query->addToOutputList( RelationalCollectionNames::clientInfoVarInCollectionLockTable() );
 
      coral::AttributeList whereDataForLock;
      whereDataForLock.extend<std::string>(
         RelationalCollectionNames::entryIDVarInCollectionLockTable() );
      whereDataForLock.begin()->data<std::string>() = "LOCK";
      string       whereClauseForLock = RelationalCollectionNames::entryIDVarInCollectionLockTable() + " = :" + RelationalCollectionNames::entryIDVarInCollectionLockTable();
      query->setCondition( whereClauseForLock, whereDataForLock );
      query->limitReturnedRows( 1, 0 );
      query->setForUpdate();
      log << coral::Debug << " Checking the LOCK table:" << RelationalCollectionNames::nameOfCollectionLockTable() << coral::MessageStream::endmsg;
      coral::ICursor& cursor = query->execute();
 
      if( !cursor.next() )    {
         log << coral::Debug << "  Creating lock ROW in " << RelationalCollectionNames::nameOfCollectionLockTable() <<  coral::MessageStream::endmsg;
         // no entry - new table or row removed by hand.  Add the row
         coral::AttributeList rowBuffer;
         rowBuffer.extend< std::string >( RelationalCollectionNames::entryIDVarInCollectionLockTable() );
         rowBuffer.extend< std::string >( RelationalCollectionNames::collectionNameVarInCollectionLockTable() );
         rowBuffer.extend< std::string >( RelationalCollectionNames::clientInfoVarInCollectionLockTable() );
         rowBuffer.extend< std::string >( RelationalCollectionNames::lockTypeVarInCollectionLockTable() );
         rowBuffer.extend< coral::TimeStamp >( RelationalCollectionNames::timestampVarInCollectionLockTable() );
 
         rowBuffer[0].data< std::string >() = "LOCK";
         rowBuffer[1].data< std::string >() = m_name;
         rowBuffer[2].data< std::string >() = userinfo();
         rowBuffer[3].data< std::string >() = XTrans? "PERM" : "TEMP";
         rowBuffer[4].data< coral::TimeStamp >() = coral::TimeStamp::now();
         lockTable.dataEditor().insertRow( rowBuffer );
 
      } else {
         string lockType =  cursor.currentRow()[RelationalCollectionNames::lockTypeVarInCollectionLockTable()].data< std::string >();
         long long lockTime =  cursor.currentRow()[RelationalCollectionNames::timestampVarInCollectionLockTable()].data< coral::TimeStamp >().total_nanoseconds() / 1000000000;
         long long nowTime = coral::TimeStamp::now().total_nanoseconds() / 1000000000;
         long long lock_age = nowTime - lockTime;
         log << coral::Debug << "  Current lock type is " << lockType << coral::MessageStream::endmsg;
         if( lockType == "PERM" ) {
            string lock_creator =  cursor.currentRow()[RelationalCollectionNames::clientInfoVarInCollectionLockTable()].data< std::string >();
            string locked_collection = cursor.currentRow()[RelationalCollectionNames::collectionNameVarInCollectionLockTable()].data< std::string >();
            log << coral::Warning << " Permanent collection database lock detected (" << lock_age << " sec old) made by " << lock_creator << " on collection " << locked_collection << coral::MessageStream::endmsg;
            if( m_lockIngoreAge==0 || lock_age < m_lockIngoreAge ) {
               delete query; query = 0;
               // release locks and wait some
               m_session.transaction().commit();

               if( total_wait >= m_maxWait ) {
                  log << coral::Warning << " Wait time exceeded: maximum specified wait time: " << m_maxWait << " sec - giving up" << coral::MessageStream::endmsg;
                  throw pool::Exception("Lock wait time exceeded",
                                        "RelationalCollection::open::DBLock::lock",
                                        "RelationalCollection" );
               }
               log << coral::Warning << " Sleeping for 10s" << coral::MessageStream::endmsg;
               sleep(10);
               total_wait += 10;
               bool forUpdate(false);
               m_session.transaction().start( forUpdate );
               continue;
               
            } else {
               log << coral::Warning << " Ignoring database lock: greater than " << m_lockIngoreAge <<  coral::MessageStream::endmsg;
            }
         }
         
         log << coral::Debug << "  Setting database lock to " << (  XTrans? "'PERM'" : "'TEMP'" ) << coral::MessageStream::endmsg;
         // update the row in the LOCK table to set databse lock and store lock type
         string    setClause =
            RelationalCollectionNames::collectionNameVarInCollectionLockTable()
            + " = '" + m_name + "'"
            + ", " + RelationalCollectionNames::clientInfoVarInCollectionLockTable()
            + " = '" + userinfo() + "'"
            + ", " +  RelationalCollectionNames::timestampVarInCollectionLockTable()
            + " = :newtime "
            + ", " +  RelationalCollectionNames::lockTypeVarInCollectionLockTable()
            + " = " + (  XTrans? "'PERM'" : "'TEMP'" );
         whereDataForLock.extend< coral::TimeStamp >( "newtime" );
         whereDataForLock[1].data< coral::TimeStamp >() = coral::TimeStamp::now();
         lockTable.dataEditor().updateRows( setClause, whereClauseForLock, whereDataForLock );
      }
      delete query;
      m_locked = true;
      return;
   }while( true );
}
 
 
void
DBLock::unlock()
{
   using namespace pool::RelationalCollection;
 
   coral::ISchema& nominalSchema = m_session.nominalSchema();
   coral::ITable& lockTable = nominalSchema.tableHandle(
      RelationalCollectionNames::nameOfCollectionLockTable() );
 
   string    setClause =
      RelationalCollectionNames::lockTypeVarInCollectionLockTable() + " = 'NONE'";
 
   coral::AttributeList whereDataForLock;
   whereDataForLock.extend<std::string>(
      RelationalCollectionNames::entryIDVarInCollectionLockTable() );
   whereDataForLock.begin()->data<std::string>() = "LOCK";
   string       whereClauseForLock = RelationalCollectionNames::entryIDVarInCollectionLockTable() + " = :" + RelationalCollectionNames::entryIDVarInCollectionLockTable();
 
   lockTable.dataEditor().updateRows( setClause, whereClauseForLock, whereDataForLock );
   m_locked = false;
}
 
