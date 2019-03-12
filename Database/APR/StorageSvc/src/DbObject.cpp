/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbObject.cpp 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  DbObject implementation
//--------------------------------------------------------------------
//
//  Package    : System (The POOL project)
//
//  Description: Generic data persistency
//
//  @author      M.Frank
//====================================================================

// Framework include files
#include "StorageSvc/DbHeap.h"
#include "StorageSvc/DbObject.h"
#include "StorageSvc/DbTypeInfo.h"
#include "StorageSvc/DbDatabase.h"
#include "StorageSvc/DbContainer.h"
#include "StorageSvc/DbInstanceCount.h"
#include <stdexcept>

using namespace std;
using namespace pool;

#ifdef POOL_HAVE_BASE
#define HEAP(x) DbObject::x
namespace pool {
  void* DbObject::operator new(size_t size)   // New operator
  { return DbHeap::allocate(size, 0, 0, 0);         }
  void DbObject::operator delete(void* ptr)   // delete operator
  { DbHeap::free(ptr, 0);                           }
  DbObject::DbObject()   {  DbInstanceCount::increment(this);  }
  DbObject::~DbObject()  {  DbInstanceCount::decrement(this);  }
}
#else
#define HEAP(x) x
#endif

/// operator new for the creation of objects using clustering hint
void* HEAP(operator new)(size_t size, 
                         DbContainer& cntH,
                         const DbTypeInfo* typ) 
{ 
  return cntH.allocate(size, typ);
}

/// C++ exception enabled compilation needs this delete operator
void HEAP(operator delete)(void *ptr, 
                           DbContainer& cntH)
{
  cntH.free(ptr);
}


/// Access to hosting object
const DbContainer& DbObjectAccessor::containedIn(const DbObject* pObj)  {
  return DbHeap::container(pObj);
}


/// Add a link entry to the Database
DbStatus DbObjectAccessor::makeObjectLink(const DbObject* pObj,
                                          const Token* pTok, 
                                          Token::OID_t& lnkH)
{
  if ( 0 != pTok )   {
    const DbDatabase& dbH = DbHeap::container(pObj).containedIn();
    return dbH.makeLink(pTok, lnkH);  
  }
  return Error;
}

/// Validate an association of an object
DbStatus DbObjectAccessor::getObjectLink( const DbObject* pObj,
                                          const Token::OID_t& lnkH,
                                          Token*        pTok)
{
  return DbHeap::container(pObj).containedIn().getLink(lnkH, pTok);
}

/// Validate an association of an object
Token::OID_t& DbObjectAccessor::objectOid(const DbObject* pObj) {
  return DbHeap::oid(pObj);
}

/// Open handle
DbStatus DbObjectAccessor::open(void**                    ptr,
                                ShapeH                    shape,
                                const DbContainer&        cntH, 
                                const Token&              tokenH, 
                                DbAccessMode )
{
  return cntH.load(ptr, shape, tokenH.oid());
}
