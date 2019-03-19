/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//  DbDomainObj handle implementation
//--------------------------------------------------------------------
//
//  Package    : StorageSvc (The POOL project)
//
//  Description: Generic data persistency
//
//  @author      M.Frank
//====================================================================

// Framework include files
#include "StorageSvc/pool.h"
#include "StorageSvc/DbSession.h"
#include "StorageSvc/IDbDomain.h"
#include "StorageSvc/IOODatabase.h"
#include "StorageSvc/DbInstanceCount.h"
#include "POOLCore/DbPrint.h"
#include "DbDatabaseObj.h"
#include "DbDomainObj.h"

// C++ include files
#include <vector>

using namespace std;
using namespace pool;

/// Constructor
DbDomainObj::DbDomainObj(const DbSession& sessionH, 
                               const DbType& typ,
                               DbAccessMode mode)
: Base("", mode, typ, sessionH.db(typ)), 
  m_session(sessionH),
  m_maxAge(2),
  m_info(0)
{
  setName("Domain["+type().storageName()+"]");
  DbInstanceCount::increment(this);
  DbPrint log(name());    
  if ( 0 == db() )    {
    log << DbPrintLvl::Error << ">   Access   DbDomain     "<<accessMode(mode)
        << " " << name() << " (UNKNOWN)" 
        << " impossible."
        << " [" << typ.storageName() << "] " 
        << DbPrint::endmsg;
    type().missingDriver(log);
    return;
  }
  m_info = db()->createDomain();
  if ( !m_session.add( this ).isSuccess() )    {
    log << DbPrintLvl::Error << ">   Access   DbDomain     "<<accessMode(mode)
        << " " << name() 
        << " (" << db()->name() << ")" 
        << " impossible. Error inserting domain!"
        << DbPrint::endmsg;
    return;
  }
  log << DbPrintLvl::Info    << ">   Access   DbDomain     "
      << accessMode(mode)
      << " [" << type().storageName() << "] " 
      << DbPrint::endmsg;
}

/// Destructor
DbDomainObj::~DbDomainObj()  {
  DbPrint log( name() );
  string dbtyp(db() ? db()->name() : string("UNKNOWN"));
  DbInstanceCount::decrement(this);
  clearEntries();
  if ( m_session.isValid() )    {
    m_session.remove (this);
  }
  deletePtr(m_info);
  log << DbPrintLvl::Info    << ">   Deaccess DbDomain     "
      << accessMode(mode()) 
      << " [" << type().storageName() << "] " 
      << DbPrint::endmsg;
}

bool DbDomainObj::existsDbase( const string& name)
{  return (m_info) ? m_info->existsDbase( name ) : false;               }

DbStatus DbDomainObj::open(DbAccessMode mod)   {
  setMode(mod);
//  return m_info ? m_info->open(session(),name(),mode()) : Error;
  return m_info ? Success : Error;
}

DbStatus DbDomainObj::open()
{  return open( mode() );                                               }

DbStatus DbDomainObj::close()   {
  if ( m_session.isValid() )    {
    vector<DbDatabaseObj*> dbs;
    for (const_iterator i = begin(); i != end(); ++i ) {
      DbDatabaseObj* obj = (*i).second;
      dbs.push_back(obj);
    }
    for (vector<DbDatabaseObj*>::iterator k=dbs.begin(); k!=dbs.end(); ++k)  {
      DbDatabaseObj* curr = (*k);
      curr->close();
      this->remove(curr);
    }
    clearEntries();
    m_session.remove(this);
    m_session = DbSession(0);
    return Success;
  }
  return Error;
}

/// Increase the age of all open databases
DbStatus DbDomainObj::ageOpenDbs() {
  if ( m_session.isValid() )    {
    for (iterator i = begin(); i != end(); ++i ) {
      DbDatabaseObj* pDB = (*i).second;
      DbAccessMode m  = pDB->mode();
      if( 0==(m&pool::CREATE) && 0==(m&pool::UPDATE) )  {
        pDB->setAge(1);
      }
    }
    return Success;
  }
  return Error;
}

/// Check if databases are present, which aged a lot and need to be closed
DbStatus DbDomainObj::closeAgedDbs()  {
  if ( m_session.isValid() )    {
    vector<DbDatabaseObj*> aged_dbs;
    for (const_iterator i = begin(); i != end(); ++i ) {
      DbDatabaseObj* pDB = (*i).second;
      if ( pDB->age() > m_maxAge )   {
        DbAccessMode m  = pDB->mode();
        if( 0 == (m&pool::CREATE) && 0 == (m&pool::UPDATE) )  {
          aged_dbs.push_back(pDB);
        }
      }
    }
    vector<DbDatabaseObj*>::const_iterator j;
    for (j=aged_dbs.begin(); j != aged_dbs.end(); ++j)
      (*j)->retire();
    return Success;
  }
  return Error;
}

/// Set domain specific options
DbStatus DbDomainObj::setOption(const DbOption& refOpt)  
{  return m_info ? m_info->setOption(refOpt) : Error;                   }

/// Access domain specific options
DbStatus DbDomainObj::getOption(DbOption& refOpt) const
{  return m_info ? m_info->getOption(refOpt) : Error;                   }
