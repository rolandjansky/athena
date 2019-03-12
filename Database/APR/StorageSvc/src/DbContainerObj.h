/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbContainerObj.h 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  DbContainerObj class definitions
//--------------------------------------------------------------------
//
//  Package    : StorageSvc  (The POOL project)
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBCONTAINEROBJ_H
#define POOL_DBCONTAINEROBJ_H 1

// Framework include files
#include "PersistentDataModel/Token.h"
#include "StorageSvc/DbObject.h"
#include "StorageSvc/DbDatabase.h"
#include "StorageSvc/DbAccessObj.h"
#include "StorageSvc/DbContainer.h"

/*
 *  POOL namespace declaration
 */
namespace pool  {

  class DataCallBack;
  typedef const class Shape        *ShapeH;

  /** @class DbContainerObj DbContainerObj.h src/DbContainerObj.h
    *
    * Description:
    *
    * Implementation independent part of a container object
    * objects.
    *
    * There is a ring of protection around the object. The object can
    * only be accessed through its handle, the technology dependent 
    * code and the DbDatabaseObj object hosting the container.
    * This should ensure proper reference counting and inhibit non
    * existing references flying around.
    *
    * @author  M.Frank
    * @version 1.0
    */
  struct DbObjectHolder  {
    DbObject* m_obj;
    DbObjectHolder(DbObject* p) : m_obj(p) {}
    int release();
  };
  class DbContainerObj : public  DbAccessObj<DbObject*, DbObjectHolder >  {
  private:
    typedef DbObjectHandle<DbObject> ObjHandle;

    /// Pointer to interface of the technology dependent part
    IDbContainer*                 m_info;
    /// Container token
    const Token*                  m_tokH;
    /// Handle to hosting Database
    DbDatabase                    m_dbH;
    /// Flag indication DbStatus of technology dependent container
    bool                          m_isOpen;

    /// Check database access
    bool hasAccess();
  public:
    /// Standard constructor of a container object using the Database handle as a clustering hint
    /** @param   dbH     [IN]    Valid handle to database object
      * @param   nam     [IN]    Name of the container to be opened.
      * @param   dbtyp   [IN]    Database type (including minor type)
      * @param   mod     [IN]    Access mode.
      *
      * @return Status code indicating success or failure.
      */
    DbContainerObj( const DbDatabase&  dbH,
                    const std::string& nam,
                    const DbType&      dbtyp,
                    DbAccessMode       mod);
    /// Standard destructor
    virtual ~DbContainerObj();
    /// Access to internals
    IDbContainer* info()  const             {  return m_info;         }
    /// Retrieve persistent type information by name
    const DbTypeInfo* objectShape(const Guid& nam) const;
    /// Add persistent type. Returns error if the type is not supported.
    DbStatus addShape(const DbTypeInfo* typ);
    /// Handle to Database (CONST)
    DbDatabase& database()                  {  return m_dbH;          }
    /// Access the token of the container object
    const Token* token() const              {  return m_tokH;         }
    /// Query the pending transaction stack
    bool updatesPending() const;
    /// Flag if container was opened
    bool isOpen() const                     {  return m_isOpen;       }
    /// Check if database is in read-only mode
    bool isReadOnly() const      
    { return !(mode()&pool::UPDATE) && !(mode()&pool::CREATE);    }
    /// Cancel transaction flag
    void cancelTransaction()                { }
    /// Size of the Database container (=# of objects)
    long long int size();
    /// Open the container
    DbStatus open(const DbTypeInfo* typ);
    /// Close the container
    DbStatus close();
    /// Retire the container
    DbStatus retire();
    /// Execute Database Transaction Action
    DbStatus transAct(Transaction::Action);
    /// Pass options to the implementation
    DbStatus setOption(const DbOption& opt);
    /// Access options
    DbStatus getOption(DbOption& refOpt);

    /// Remove the transient representation of the object from memory
    DbStatus remove(ObjHandle& objH);
    /// Destroy an existing persistent object identified by its handle
    DbStatus destroy(const Token::OID_t& linkH);
    /// Add an object to the container identified by its handle
    DbStatus save(const DbObjectHandle<DbObject>& objH,
                  const DbTypeInfo* typ);


    //@{

    /// In place allocation of raw memory
    void* allocate(unsigned long siz, DbContainer& cntH, ShapeH shape);

    /// In place allocation of object location
    DbStatus allocate(DbContainer& cntH, const void* object, ShapeH shape, Token::OID_t& oid);

    /// In place free of raw memory
    DbStatus free(void* ptr, DbContainer& cntH);

    /// Save new object in the container and return its handle
    /** @param  cntH      [IN]   Handle to container object.
      * @param  linkH     [OUT]  Internal OID to identify object.
      *
      * @return DbStatus code indicating success or failure.
      */
    DbStatus save(DbContainer& cntH, const void* object, ShapeH shape, Token::OID_t& linkH);

    /// Update an object to the container identified by its handle.
    /** @param  cntH      [IN]   Handle to container object.
      * @param  linkH     [IN]   Internal OID to identify object.
      *
      * @return DbStatus code indicating success or failure.
      */
    DbStatus update(DbContainer& cntH, const void* object, ShapeH shape, const Token::OID_t& linkH);

    /// Update existing object in the container
    /** @param cntH      [IN]     Valid handle to container 
      * @param objH      [IN]     Object handle
      *
      * @return Status code indicating success or failure.
      */
    DbStatus update(DbContainer& cntH, const void* object, ShapeH shape, const DbObjectHandle<DbObject>& objH);

    /// Select object in the container identified by its handle
    DbStatus load(DataCallBack* call, 
                  const Token::OID_t& linkH, 
                  Token::OID_t&       oid,
                  bool          any_next);
    //@}

    /// Perform UPDATE select
    DbStatus update(DbSelect& sel);
    /// Perform DELETE statement
    DbStatus destroy(DbSelect& sel);
    /// Perform selection. The statement belongs to the container afterwards.
    DbStatus select(DbSelect& sel);
    /// Fetch next object address of the selection to set token
    DbStatus fetch(DbSelect& sel);
  };
}       // End namespace pool
#endif  // POOL_DBCONTAINEROBJ_H
