/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//  DbContainer handle definitions
//--------------------------------------------------------------------
//
//  Package    : StorageSvc  (The POOL project)
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBCONTAINER_H
#define POOL_DBCONTAINER_H

// Framework include files
#include "PersistentDataModel/Token.h"
#include "StorageSvc/DbObject.h"
#include "StorageSvc/DbHandleBase.h"
#include "StorageSvc/Transaction.h"

/*
 * POOL namespace declaration
 */
namespace pool  {

  // Forward declarations
  class IOODatabase;
  class IDbContainer;
  class DbSelect;
  class DbDatabase;
  class DbTypeInfo;
  class DbContainerObj;
  class DbTransaction;
  class DbOption;
  template <class T> class DbObjectHandle;

  typedef const class Shape        *ShapeH;

  /** @class DbContainer DbContainer.h StorageSvc/DbContainer.h
    *
    * Description:
    * Definition of the object describing a database container
    * Handle managing a DbContainerObj, which is a container of persistent
    * objects.
    *
    * @author  M.Frank
    * @version 1.0
    */
  class DbContainer : public DbHandleBase<DbContainerObj>  {
  private:
    /// Assign transient object properly (including reference counting)
    void switchPtr( const DbContainerObj* obj)  const;
    /// Load object in the container identified by its link handle
    DbStatus _load( DbObjectHandle<DbObject>& objH,
                    const Token::OID_t& linkH,
                    const DbTypeInfo* typ,
                    bool any_next);
    /// Load next object in the container identified by its link handle
    DbStatus _loadNext(DbObjectHandle<DbObject>& objH,
                    Token::OID_t& linkH,
                    const DbTypeInfo* typ);
    /// Internal add of an object entry identified by its handle
    DbStatus _save( const DbObjectHandle<DbObject>& objH,
                    const DbTypeInfo* typ);
    /// Add object to the container
    DbStatus _update( const DbObjectHandle<DbObject>& handle,
                      const DbTypeInfo* typ);
    /// Remove the transient representation of the object from memory
    DbStatus _remove( DbObjectHandle<DbObject>& objH);
    /// Destroy the persistent representation of the object
    DbStatus _destroy(DbObjectHandle<DbObject>& objH);

  public:
    /// Constructor with initializing arguments
    DbContainer(const DbType& typ=POOL_StorageType) { m_type=typ;          }
    /// Copy constructor
    DbContainer(const DbContainer& c) : Base()  { switchPtr(c.m_ptr);      }
    /// Constructor taking transient object
    DbContainer (const DbContainerObj* ptr)     { switchPtr(ptr);          }
    /// Standard Destructor
    virtual ~DbContainer()                      { switchPtr(0);            }
    /// Assignment operator
    DbContainer& operator=(const DbContainer& copy) {
      if ( &copy != this )  {
        m_type = copy.m_type;
        switchPtr( copy.m_ptr );
      }
      return *this;
    }
    /// Assignment operator to reset the handle easily using 0
    DbContainer& operator=(const int /* nuller */ ) {
      switchPtr(0);
      return *this;
    }
    /// Access reference counter
    int refCount() const;
    /// Name of the container the handle is supposed to point to
    const std::string& name() const;
    /// Mode of the handle (READ,WRITE,...)
    DbAccessMode openMode() const;
    /// Access to the size of the container
    long long int size()    const;
    /// Access to the Database the container resides in
    const DbDatabase& containedIn() const;
    /// Allow access to the Database implementation
    IOODatabase* db() const;
    /// Let the implementation access the internals
    IDbContainer* info()  const;
    /// Retrieve persistent type information by name
    const DbTypeInfo* objectShape(const Guid& nam) const;
    /// Access the token of the container object
    const Token* token() const;
    /// Close the container the handle poits to
    DbStatus close()  const;

    /// Open the container residing in \<file\> with given name and access mode
    /** @param   dbH     [IN]    Valid handle to database object
      * @param   nam     [IN]    Name of the container to be opened.
      * @param   typ     [IN]    Type information in the event the container
      *                          must be created.
      * @param   dbtyp   [IN]    Database type (including minor type)
      * @param   mod     [IN]    Access mode.
      *
      * @return Status code indicating success or failure.
      */
    DbStatus open(const DbDatabase&   dbH, 
                  const std::string&  nam, 
                  const DbTypeInfo*   typ, 
                  const DbType&       dbtyp,
                  DbAccessMode        mod) const;

    /// Check if the container was opened
    bool isOpen() const;
    /// Execute Database Transaction Action
    DbStatus transAct(Transaction::Action action);
    /// Pass options to the implementation
    DbStatus setOption(const DbOption& refOpt);
    /// Access options
    DbStatus getOption(DbOption& refOpt) const;

    /** Access objects through select staements.                            
      * This access type is ideal for relational Databases.
      * Other technologies only have very limited support for this
      * interface.
      */
    //@{ 
    /// Perform DELETE statement
    DbStatus destroy(DbSelect& sel);
    /// Perform UPDATE statement
    DbStatus update(DbSelect& sel);
    /// Perform selection. The statement belongs to the container afterwards.
    DbStatus select(DbSelect& sel) const;
    /// Fetch next object address of the selection to set token
    DbStatus fetch(DbSelect& sel) const;
    //@}

    /** Access objects using pointer and shape
      */
    //@{
    /// In place allocation of object location
    DbStatus allocate(const void* object, ShapeH shape, Token::OID_t& oid);
    /// Save new object in the container and return its handle
    DbStatus save(const void* object, ShapeH shape, Token::OID_t& linkH);
    /// Update an object to the container identified by its handle
    DbStatus update(const void* object, ShapeH shape, const Token::OID_t& linkH);
    /// Destroy an existing persistent object identified by its handle
    DbStatus destroy(const Token::OID_t& linkH);
    /// Select object in the container identified by its handle
    DbStatus load(void** ptr, ShapeH shape, const Token::OID_t& lH) const;
    //@}

    /** Access objects by handle directly.                                  
        This is the generic "direct" object access.
    */
    //@{ 
    /// In place allocation of raw memory
    void* allocate(unsigned long siz, const DbTypeInfo* typ);
    /// In place free of raw memory
    DbStatus free(void* ptr);
    /// Remove the transient representation of the object from memory
    template <class T> DbStatus remove( const DbObjectHandle<T>& objH)
    { DbObjectHandle<DbObject> oH(objH.ptr()); return _remove(oH);           }
    /// Destroy the persistent representation of the object
    template <class T> DbStatus destroy(const DbObjectHandle<T>& objH) 
    { DbObjectHandle<DbObject> oH(objH.ptr()); return _destroy(oH);          }
    /// Add an object to the container identified by its handle
    template <class T> DbStatus save( const DbObjectHandle<T>& objH,
                                      const DbTypeInfo* typ)    
    { DbObjectHandle<DbObject> oH(objH.ptr()); return _save(oH, typ);        }
    /// Update an object to the container identified by its handle
    template <class T> DbStatus update( const DbObjectHandle<T>& objH,
                                        const DbTypeInfo* typ) 
    { DbObjectHandle<DbObject> oH(objH.ptr()); return _update(oH, typ);      }
    /// Load object in the container identified by its handle
    template <class T> DbStatus load( DbObjectHandle<T>& objH,
                                      const Token::OID_t& linkH,
                                      const DbTypeInfo* typ)
    { return _load(objH, linkH, typ, false);                                 }
    /// Load object in the container identified by its handle
    template <class T> DbStatus loadNext(DbObjectHandle<T>& objH,
                                         Token::OID_t& linkH,
                                         const DbTypeInfo* typ)
    { return _loadNext(objH, linkH, typ);                                    }
    //@}
  };
}       // End namespace pool
#endif  // POOL_DBCONTAINER_H
