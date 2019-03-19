/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IDbContainer.h 726071 2016-02-25 09:23:05Z krasznaa $
//
//  Package    : pool/StorageSvc (The pool framework)
//
//  @author      M.Frank
//
//====================================================================
#ifndef POOL_IDBCONTAINER_H
#define POOL_IDBCONTAINER_H

// Framework include files
#include "PersistentDataModel/Token.h"
#include "StorageSvc/DbObject.h"
#include "StorageSvc/DbSection.h"
#include "StorageSvc/Transaction.h"

#include <vector>

/*
 *   POOL namespace declaration
 */
namespace pool    {

  // Forward declarations
  class DbTypeInfo;
  class DbDatabase;
  class DbContainer;
  class DataCallBack;
  class DbOption;
  class DbSelect;

  typedef const class Shape        *ShapeH;

  /** @class IDbContainer IDbContainer.h StorageSvc/IDbContainer.h 
    *
    * Description:
    *
    * Interface to the implementation specific part of a container object
    * objects.
    *
    * @author  M.Frank
    * @version 1.0
    */
  class IDbContainer    {
  public:
    /// Sections definition
    typedef std::vector<DbSection>  Sections;
  protected:
    /// Destructor (called only by sub-classes)
    virtual ~IDbContainer()   {     }

  public:
    /// Release the technology specific implementation
    virtual void release() = 0;
    /// Access to container size
    virtual long long int size() = 0;
    /// Ask if a given shape is supported
    virtual DbStatus isShapeSupported(const DbTypeInfo* typ) const = 0;
    /// Set options
    virtual DbStatus setOption(const DbOption& refOpt) = 0;
    /// Access options
    virtual DbStatus getOption(DbOption& refOpt) const = 0;
    /// In place allocation of raw memory
    virtual void* allocate( unsigned long siz, 
                            DbContainer&  cntH,
                            ShapeH shape) = 0; 
    /// In place allocation of object location
    virtual DbStatus allocate(DbContainer& cntH, 
                              const void* object,
                              ShapeH shape, 
                              Token::OID_t& oid) = 0;
    /// In place deletion of raw memory
    virtual DbStatus free(  void* ptr,
                            DbContainer&  cntH) = 0;

    /// Number of next record in the container (=size if no delete is allowed)
    virtual long long int nextRecordId() = 0;

    /// Close the container
    virtual DbStatus close() = 0;
    /// Open the container
    virtual DbStatus open(  const DbDatabase& dbH, 
                            const std::string& nam, 
                            const DbTypeInfo* info, 
                            DbAccessMode mode) = 0;
    /// Perform UPDATE select
    virtual DbStatus update(  DbSelect& sel) = 0;
    /// Perform DELETE select
    virtual DbStatus destroy( DbSelect& sel) = 0;
    /// Define selection criteria
    virtual DbStatus select(  DbSelect& criteria) = 0;
    /// Fetch next object address of the selection to set token
    virtual DbStatus fetch(DbSelect& sel) = 0;

    /// Find object within the container and load it into memory
    /** @param  call      [IN]   Callback to load data
      * @param  linkH     [IN]   Preferred object OID
      * @param  oid      [OUT]   Actual object OID
      * @param  mode      [IN]   Object access mode
      * @param  any_next  [IN]   On selection, objects may be skipped.
      *                          If objects are skipped, the actual oid
      *                          will differ from the preferred oid.
      * @return Status code indicating success or failure.
      */
    virtual DbStatus load(  DataCallBack* call, 
                            const Token::OID_t& linkH,
                            Token::OID_t&       oid,
                            DbAccessMode  mode,
                            bool          any_next=false) = 0;

    /// Save new object in the container and return its handle
    /** @param  cntH      [IN]   Handle to container object.
      * @param  object    [IN]   Pointer to feed data.
      * @param  linkH     [OUT]  Internal OID to identify object.
      *
      * @return DbStatus code indicating success or failure.
      */
    virtual DbStatus save(  DbContainer&  cntH,
                            const void* object,
                            ShapeH shape, 
                            Token::OID_t& linkH) = 0;

    /// Add object to the container
    virtual DbStatus save(  const DbObjectHandle<DbObject>& objH) = 0;
    /// Update existing object in the container
    /** @param cntH      [IN]     Valid handle to container 
      * @param  object   [IN]     Pointer to feed data.
      * @param objH      [IN]     Object handle
      *
      * @return Status code indicating success or failure.
      */
    virtual DbStatus update(DbContainer&  cntH,
                            const void* object,
                            ShapeH shape, 
                            const DbObjectHandle<DbObject>& objH) = 0;

    /// Update existing object in the container
    /** @param cntH      [IN]     Valid handle to container 
      * @param  object   [IN]     Pointer to feed data.
      * @param linkH     [IN/OUT] Object identifier
      *
      * @return Status code indicating success or failure.
      */
    virtual DbStatus update(DbContainer&  cntH,
                            const void* object,
                            ShapeH shape, 
                            const Token::OID_t& linkH) = 0;

    /// Destroy an object in a container
    /** @param linkH     [IN]     Object identifier
      *
      * @return Status code indicating success or failure.
      */
    virtual DbStatus destroy( const Token::OID_t& linkH) = 0;

    /// Execute Transaction Action
    virtual DbStatus transAct(Transaction::Action) = 0;

    /// Query the pending transaction stack
    virtual bool updatesPending() const = 0;

    /// Attach sections to container object
    virtual void setSections(const Sections& sections) = 0;
  };
}      // End namespace pool
#endif // POOL_IDBCONTAINER_H
