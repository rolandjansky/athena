/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbContainerImp.h 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//
//  Package    : StorageSvc (The POOL project)
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBCONTAINERIMP_H
#define POOL_DBCONTAINERIMP_H 1

/// Framework include files
#include "PersistentDataModel/Token.h"
#include "StorageSvc/DbObject.h"
#include "StorageSvc/IDbContainer.h"
#include "StorageSvc/DbImplementation.h"

// STL include files
#include <map>
#include <vector>
#include <utility>

/*
 * POOL namespace declaration
 */
namespace pool    {

  // Forward declarations
  class IDbContainer;

  /** @class DbContainerImp DbContainerImp.h StorageSvc/DbContainerImp.h
    *
    *  "Generic" Container implementation
    *
    *  Description: Generic helper class to implement stuff common to all
    *  existing Database containers. The base implementations can allways 
    *  be overwritten.
    *
    *  @author  M.Frank
    *  @version 1.0
    */
  class DbContainerImp : public DbImplementation, 
                         virtual public IDbContainer
  {
  protected:
    /// Transaction Fifo definition
    class _Transaction {
    public:
      mutable DbObject*        objH;
      DataCallBack*            call;
      Token::OID_t             link;
      AccessMode               action;
      _Transaction()
        : objH(0), call(0), action(NONE) {}
      _Transaction(DbObject* o, 
                   DataCallBack* c,
                   const Token::OID_t& l,
                   AccessMode    a) 
      : objH(o), call(c), link(l), action(a) {}
      _Transaction(const _Transaction& c)
      : objH(c.objH), call(c.call), link(c.link), action(c.action) { c.objH = 0; }
      _Transaction& operator= (const _Transaction& c) = delete;
      ~_Transaction() {}
      void assign(DbObject* o, DataCallBack* c, const Token::OID_t& l, AccessMode a) {
        objH = o; call = c; link = l; action = a;
      }
    };
    typedef std::vector < _Transaction > TransactionStack;
  private:
    /// Transaction fifo storage for writing
    TransactionStack      m_stack;
    /// Current size of the transaction stack
    size_t                m_size;
    /// Number of objects to be written out during open transaction
    size_t                m_writeSize;
    /// Accumulated stack entry types
    int                   m_stackType;
  protected:
    /// Container name
    std::string           m_name;
    /// Flag to indicate if object updates are supported
    bool                  m_canUpdate;
    /// Flag to indicate if object removals are supported
    bool                  m_canDestroy;
    /// Section buffer for merged tables
    Sections              m_sections;

    /// Standard destructor
    virtual ~DbContainerImp();
    /// Access accumulated stack entry types
    int stackType()   const
    { return m_stackType;                                                     }
    /// Access stack size
    size_t stackSize()  const   
    { return m_size;                                                          }
    /// Query the pending transaction stack
    virtual bool updatesPending() const 
    { return m_size > 0;                                                      }
    /// Internal: get access to stack entry
    TransactionStack::value_type* stackEntry(size_t which) 
    { return (which <= m_size) ? &(*(m_stack.begin()+which)) : 0;             }
    /// Destroy persistent object in the container; does not touch transient!
    virtual DbStatus destroyObject(TransactionStack::value_type& /* entry */)  
    { return Error;                                                   }
    /// Update persistent object in the container
    virtual DbStatus updateObject(TransactionStack::value_type& /* entry */)  
    { return Error;                                                   }
    /// Commit single entry to container
    virtual DbStatus writeObject(TransactionStack::value_type& /* entry */)  
    { return Error;                                                   }
    /// Execute object modification requests during a transaction
    /** @param refTr    [IN]  Transaction reference
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus commitTransaction(DbTransaction& refTr);
    /// Execute end of object modification requests during a transaction
    /** @param refTr    [IN]  Transaction reference
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus endTransaction(DbTransaction& refTr);

    /// Access section identifier from OID
    virtual const DbSection& getSection(const Token::OID_t& oid) const;

  public:
    /// Constructor with initializing arguments
    DbContainerImp(IOODatabase* db);
    /// Release instance (Abstract interfaces do not expose destructor!)
    virtual void release()                             { delete this;           }
    /// Attach sections to container object
    virtual void setSections(const Sections& sections) { m_sections = sections; }
    /// Size of the container
    virtual long long int size();
    /// Number of next record in the container (=size if no delete is allowed)
    virtual long long int nextRecordId();
    /// Close the container and deallocate resources
    virtual DbStatus close();

    /// Access options
    /** @param opt      [IN]  Reference to option object.
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus getOption(DbOption& opt) const;

    /// Set options
    /** @param opt      [IN]  Reference to option object.
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus setOption(const DbOption& opt);

    /// Start/Commit/Rollback Database Transaction
    virtual DbStatus transAct(DbTransaction& pTransaction);
    /// In place allocation of raw memory for the transient object
    virtual void* allocate(   unsigned long siz, 
                              DbContainer&  cntH,
                              ShapeH shape);
    /// In place allocation of object location
    virtual DbStatus allocate(DbContainer& cntH, 
                              const void* object,
                              ShapeH shape,
                              Token::OID_t& oid);
    /// In place deletion of raw memory
    virtual DbStatus free(    void* ptr,
                              DbContainer& cntH);
    /// Perform UPDATE statement
    virtual DbStatus update(DbSelect&  /* sel */)     { return Error;   }
    /// Perform DELETE statement
    virtual DbStatus destroy(DbSelect& /* sel */)     { return Error;   }
    /// Fetch next object address of the selection to set token
    virtual DbStatus fetch(DbSelect&      sel);
    /// Add the specified object to the delete stack.
    virtual DbStatus destroy(const Token::OID_t& lnkH);
    /// Update existing object in the container
    /** @param cntH      [IN]     Valid handle to container 
      * @param object    [IN]     Data object
      * @param linkH     [IN/OUT] Object identifier
      *
      * @return Status code indicating success or failure.
      */
    virtual DbStatus update(DbContainer&  cntH,
                            const void* object,
                            ShapeH shape,
                            const Token::OID_t& linkH);
    /// Update existing object in the container
    /** @param cntH      [IN]     Valid handle to container 
      * @param object    [IN]     Data object
      * @param objH      [IN]     Object handle
      *
      * @return Status code indicating success or failure.
      */
    virtual DbStatus update(DbContainer&  cntH,
                            const void* object,
                            ShapeH shape,
                            const DbObjectHandle<DbObject>& objH);
    /// Add single entry to container
    virtual DbStatus save(  const DbObjectHandle<DbObject>& objH);

    /// Save new object in the container and return its handle
    /** @param  cntH      [IN]   Handle to container object.
      * @param  object    [IN]   Data object
      * @param  linkH     [OUT]  Internal OID to identify object.
      * @return DbStatus code indicating success or failure.
      */
    virtual DbStatus save(DbContainer&  cntH,
                          const void* object,
                          ShapeH shape,
                          Token::OID_t& linkH);

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
    virtual DbStatus load(DataCallBack* call,
                          const Token::OID_t& lnkH, 
                          Token::OID_t&       oid,
                          DbAccessMode        mod,
                          bool                any_next);
    /// Clear Transaction stack containing transaction requests
    virtual DbStatus clearStack();
    /// Fetch refined object address. Default implementation returns identity
    virtual DbStatus fetch(const Token::OID_t& linkH, Token::OID_t& stmt);
    /// Find object by object identifier and load it into memory
    /** @param  call      [IN]   Callback to load data
      * @param  oid      [OUT]   Object OID
      * @param  mode      [IN]   Object access mode
      *
      * @return Status code indicating success or failure.
      */
    virtual DbStatus loadObject(DataCallBack* call,
                                Token::OID_t& oid,
                                DbAccessMode  mode);

  };
}       // End namespace pool
#endif  // POOL_DBCONTAINERIMP_H
