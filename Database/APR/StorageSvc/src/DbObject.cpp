/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

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
#include "CxxUtils/checker_macros.h"
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
DbStatus DbObjectAccessor::makeObjectLink ATLAS_NOT_THREAD_SAFE
  (const DbObject* pObj,
   Token* pTok, 
   Token::OID_t& lnkH)
{
  if ( 0 != pTok )   {
    DbDatabase& dbH = DbHeap::container(const_cast<DbObject*>(pObj)).containedIn();
    return dbH.makeLink(pTok, lnkH);  
  }
  return Error;
}

/// Validate an association of an object
Token::OID_t& DbObjectAccessor::objectOid(DbObject* pObj) {
  return DbHeap::oid(pObj);
}

/// Validate an association of an object
const Token::OID_t& DbObjectAccessor::objectOid(const DbObject* pObj) {
  return DbHeap::oid(pObj);
}

/// Read an object from a container
DbStatus DbObjectAccessor::read( void**                  ptr,
                                 ShapeH                  shape,
                                 DbContainer&            cntH, 
                                 const Token::OID_t&     oid,
                                 int                     /* merge_section_n */ )
{
   return cntH.load(ptr, shape, oid);
}
