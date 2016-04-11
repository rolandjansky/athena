/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbCallBack.h 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  DbCallBack call back class definitions
//--------------------------------------------------------------------
//
//  Package    : StorageSvc  (The POOL project)
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBCALLBACK_H
#define POOL_DBCALLBACK_H 1

// Framework include files
#include "StorageSvc/DbStatus.h"
#include "StorageSvc/DbObject.h"
#include "StorageSvc/DataCallBack.h"

/* 
 * POOL namespace declaration
 */
namespace pool  {

  // Forward declarations
  class DbContainer;
  class DbTypeInfo;

  struct DbObjectEnv    {
    const DbContainer*              cntH;
    const DbTypeInfo*               typH;
    /// Constructor with initializing arguments
    DbObjectEnv(const DbContainer* pCntH, 
                const DbTypeInfo* pType)
    : cntH(pCntH), typH(pType) {}
    /// Copy Constructor.
    DbObjectEnv(const DbObjectEnv& c) 
    : cntH(c.cntH), typH(c.typH) {}
    /// Standard Destructor
    virtual ~DbObjectEnv() {}

    DbObjectEnv& operator= (const DbObjectEnv&) = delete;

  };

  /** @class DbCallBack DbCallBack.h StorageSvc/DbCallBack.h
    *
    * Description of class DbCallBack:
    * Definition of the class, which is able to read objects from the 
    * Database on an "as is" bases, i.e. in the format they were written.
    * This implicitly assumes, that the transient corresponds to the
    * persistent object mapping and that objects are sub-classes of the
    * DbObject class.
    *
    * @author  M.Frank
    * @version 1.0
    */
  class DbCallBack : public DataCallBack  {
  private:
    /// Object environment
    DbObjectEnv   m_env;
    /// Object pointer
    DbObject*     m_pObject;
  public:

    /// Constructor with initializing arguments
    DbCallBack(const DbContainer*        pCntH,
               DbObjectHandle<DbObject>* pObjH,
               const DbTypeInfo*         pType);
    /// Copy Constructor.
    DbCallBack(const DbCallBack& copy);
    /// Standard Destructor
    virtual ~DbCallBack();

    DbCallBack& operator= (const DbCallBack&) = delete;

    /// Pass object for conversion data to the callback
    /**    @param ptr            [IN] Reference to object
    */
    virtual void setObject(const void* ptr)     { m_pObject = (DbObject*)ptr;}

    /// Pass shape object for conversion data to the callback
    /**    @param shape          [IN] Reference to shape object
    */
    virtual void setShape(ShapeH shape)         
    { m_env.typH = (const DbTypeInfo*)shape;                                 }

    /// Retrieve shape object from the callback
    virtual ShapeH shape()  const               { return ShapeH(m_env.typH); }

    /// Retrieve object from the callback
    virtual const void* object()  const         { return m_pObject;          }

    /// Callback when a read sequence should be started
    /** @param action_type    [IN] Action type: PUT/GET
      * @param data           [IN] Reference to persistent data location (on Write)
      * @param context        [IN] Data context for subsequent calls
      *
      * @return DbStatus indicating success or failure.
      */
    virtual DbStatus start(CallType action_type, void* data, void** context);

    /// Callback to retrieve the absolute address of a column
    /** @param action_type    [IN] Action type: PUT/GET
      * @param col_identifier [IN] Reference to persistent column to be processed.
      * @param col_number     [IN] Number of persistent column.
      * @param context        [IN] Data context returned by "start"
      * @param data          [OUT] Reference to data location
      *
      * @return DbStatus indicating success or failure.
      *         If SKIP is returned, the column will not be processed.
      */
    virtual DbStatus bind(CallType             action_type,    // GET or PUT
                          const DbColumn*      col_identifier, // Col ID
                          int                  col_number,     // Col Number
                          void*                context,        // Start context
                          void**               data);          // data pointer

    /// Callback when a read sequence should be started
    /** @param action    [IN] Action type: PUT/GET
      * @param context   [IN] Data context returned by "start"
      *
      * @return DbStatus indicating success or failure.
      */
    virtual DbStatus end(CallType action, void* context);
  };
}      // End namespace pool
#endif // POOL_DBCALLBACK_H
