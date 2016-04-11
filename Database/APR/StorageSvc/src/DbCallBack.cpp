/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbCallBack.cpp 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  DbCallBack object implementation
//--------------------------------------------------------------------
//
//  Package    : System (The POOL project)
//
//  Description: Generic data persistency
//
//  @author      M.Frank
//====================================================================

#include "StorageSvc/DbHeap.h"
#include "StorageSvc/DbReflex.h"
#include "StorageSvc/DbObject.h"
#include "StorageSvc/DbColumn.h"
#include "StorageSvc/DbCallBack.h"
#include "StorageSvc/DbTypeInfo.h"
#include "StorageSvc/DbInstanceCount.h"

#include "DataModelRoot/RootType.h"


using namespace pool;
using namespace std;

typedef DbObjectHandle<DbObject> ObjHandle;
static DbInstanceCount::Counter* s_count = DbInstanceCount::getCounter(typeid(DbCallBack));

/// Standard Destructor
DbCallBack::DbCallBack( const DbContainer*        pCntH,
                        DbObjectHandle<DbObject>* pObjH,
                        const DbTypeInfo*         pType)
: m_env(pCntH, pType), 
  m_pObject(pObjH)
{
  s_count->increment();
}

/// Standard Destructor
DbCallBack::DbCallBack(const DbCallBack& copy) 
: DataCallBack(copy), m_env(copy.m_env), 
  m_pObject(copy.m_pObject)
{
  s_count->increment();
}

/// Standard Destructor
DbCallBack::~DbCallBack()  {
  s_count->decrement();
}

/// Callback when a read sequence should be started
DbStatus DbCallBack::start(CallType action_type, void* pObj, void** data_pointer)  {
  switch(action_type)  {
  case GET:
    if ( m_env.typH && m_env.cntH )  {
      DbContainer* pCnt = const_cast<DbContainer*>(m_env.cntH);
      TypeH cl = m_env.typH->clazz();
      if ( cl )   {
        DbObject* ptr = DbHeap::allocate(cl.SizeOf(), pCnt, 0, 0);
        if ( ptr )   {
#         if ROOT_VERSION_CODE < ROOT_VERSION(5,99,0) 
             cl.Construct(TypeH(0), vector<void*>(), ptr);
#         else                      
             ptr = cl.Construct(ptr); //Fix for DbString reading
#         endif             
          m_pObject = ptr;
          *data_pointer = ptr;
          return Success;
        }
      }
    }
    return Error;
  case PUT:
    *data_pointer = pObj;
    return Success;
  default:
    return Error;
  }
}

/// Callback to retrieve the absolute address of a column
DbStatus DbCallBack::bind(CallType     /* action_type */, // GET or PUT
                          const DbColumn* col_identifier, // Col ID
                          int          /* col_number  */, // Col number
                          void*           context,        // Data context
                          void**          data_pointer)   // data pointer
{
  *data_pointer = ((char*)context)+col_identifier->offset();
  return Success;
}

/// Callback when a read sequence should be started
DbStatus DbCallBack::end(CallType /* action */, void* /* context */) {
  return Success;
}
