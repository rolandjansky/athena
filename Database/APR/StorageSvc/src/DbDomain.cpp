/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbDomain.cpp 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  DbDomainObj handle implementation
//--------------------------------------------------------------------
//
//  Package    : StorageSvc  (The POOL project)
//  @author      M.Frank
//====================================================================

// Framework include files
#include "StorageSvc/DbDomain.h"
#include "StorageSvc/DbSession.h"
#include "StorageSvc/DbObject.h"
#include "DbDatabaseObj.h"
#include "DbDomainObj.h"

using namespace std;
using namespace pool;

static const string s_empty = "";

/// Object constructor
DbDomain::DbDomain(const DbDomainObj* dom)   {
  switchPtr( dom );
  if ( dom ) setType(dom->type());
}

DbStatus DbDomain::open(const DbSession& sesH,const DbType& typ,DbAccessMode mod)  const  {
  if ( !isValid() )   {
    if ( sesH.isValid() )    {
      DbType db_typ(typ.majorType());
      DbDomainObj* dom = sesH.find(db_typ);
      if ( !dom )   {
        dom = new DbDomainObj(sesH, db_typ, mod);
      }
      switchPtr(dom);
      return dom->open(mod);
    }
    close();
    return Error;
  }
  return Success;
}

DbStatus DbDomain::close()   const    {
  if ( isValid() )    {
    ptr()->close();
    switchPtr(0);
  }
  return Success;
}

/// Assign transient object properly (including reference counting)
void DbDomain::switchPtr(const DbDomainObj* obj) const {
   if( obj ) obj->addRef();
   if( isValid() ) {
      if (ptr()->release() == 0) {
         setPtr(0);
         setType(DbType(0));
      }
   }
   if( obj )  {
      setPtr(const_cast<DbDomainObj*>(obj));
      setType(obj->type());
   } else {
      setPtr(0);
      setType(DbType(0));
   }
}

/// Add domain to session
DbStatus DbDomain::add(const string& nam, DbDatabaseObj* dbH) const {
  DbStatus sc = Error;
  if ( isValid() && dbH )    {
    sc = ptr()->add(nam, dbH);
    if ( sc.isSuccess() )  {
      ptr()->addRef();
    }
  }
  return sc;
}

/// Find domain in session
DbStatus DbDomain::remove(DbDatabaseObj* dbH) const {
  DbStatus sc = Error;
  if ( isValid() && dbH )    {
    sc = ptr()->remove(dbH);
    if ( isValid() && sc.isSuccess() )  {
      if (ptr()->release() == 0) {
        setPtr(0);
        setType(DbType(0));
      }
    }
  }
  return sc;
}

/// Check for the existence of a domain within a session
bool DbDomain::exist(const DbSession& sH, int typ) const
{  return sH.find(typ) != 0;                                            }

DbSession DbDomain::containedIn() const 
{  return isValid() ? ptr()->session() : DbSession();                   }

/// Access to access mode
DbAccessMode DbDomain::openMode() const
{  return isValid() ? ptr()->mode() : DbAccessMode(pool::NOT_OPEN);     }

/// Access to domain name
const string& DbDomain::name() const
{  return isValid() ? ptr()->name() : s_empty;                          }

/// Add reference count to object if present
int DbDomain::refCount() const
{  return isValid() ? ptr()->refCount() : int(INVALID);                 }

/// Find domain in session
DbDatabaseObj* DbDomain::find(const string& db_name) const 
{  return isValid() ? ptr()->find(db_name) : 0;                         }

/// Check if Database exists within the domain
bool DbDomain::existsDbase(const string& nam) const
{  return isValid() ? ptr()->existsDbase(nam) : false;                  }

/// Access implementation internals
IDbDomain* DbDomain::info()   const
{  return isValid() ? ptr()->info() : 0;                                }

/// Allow access to the Database implementation
IOODatabase* DbDomain::db()  const
{  return isValid() ? ptr()->db() : 0;                                  }

/// Increase the age of all open databases
DbStatus DbDomain::ageOpenDbs() const
{  return isValid() ? ptr()->ageOpenDbs() : Error;                      }

/// Check if databases are present, which aged a lot and need to be closed
DbStatus DbDomain::closeAgedDbs()   const
{  return isValid() ? ptr()->closeAgedDbs() : Error;                    }

/// Set the maximal allowed age limit for files in this domain
void DbDomain::setAgeLimit(int value)
{  if ( isValid() ) ptr()->setAgeLimit(value);                          }

/// Access the maximal age limit
int DbDomain::ageLimit()  const
{  return isValid() ? ptr()->ageLimit() : -1;                           }

/// Set domain specific options
DbStatus DbDomain::setOption(const DbOption& refOpt)
{  return isValid() ? ptr()->setOption(refOpt) : Error;                 }

/// Access domain specific options
DbStatus DbDomain::getOption(DbOption& refOpt) const
{  return isValid() ? ptr()->getOption(refOpt) : Error;                 }
