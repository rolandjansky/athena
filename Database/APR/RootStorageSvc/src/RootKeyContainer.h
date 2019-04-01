/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//        Root Database implementation
//--------------------------------------------------------------------
//
//        Package    : RootStorageSvc (The POOL project)
//
//        Author     : M.Frank
//====================================================================
#ifndef POOL_ROOTKEYCONTAINER_H
#define POOL_ROOTKEYCONTAINER_H 1

// Framework include files
#include "StorageSvc/DbDatabase.h"
#include "StorageSvc/DbContainerImp.h"

// Forward declarations
namespace CINT { class IOHandler; }
class TDirectory;
class TBuffer;
class TClass;

namespace pool  { 
  // Forward declarations
  union RootDataPtr;
  class RootKeyIOHandler;

  /** @class RootKeyContainer RootKeyContainer.h src/RootKeyContainer.h
    *
    * Description:
    * ROOT specific implementation of Database container.
    * Since objects in root are stored in "trees with branches",
    * this object corresponds top a tuple (Tree/Branch), where
    * each object type (determined by the location of the transient
    * object within the data store) is accessed by the "Event" number
    * inside its tree.
    *
    * @author  M.Frank
    * @date    1/8/2002
    * @version 1.0
    */
  class RootKeyContainer : public DbContainerImp  {
    /// Reference to the root tree object
    TDirectory*        m_dir;
    /// Parent Database handle
    DbDatabase         m_dbH;
    /// Root database file reference
    RootDatabase*         m_rootDb;
    /// CINT IO handler to allow user overloads....
    RootKeyIOHandler*  m_ioHandler;
    /// Policy flag
    int                m_policy;
    /// Number of bytes written/read during last operation. Set to -1 if it failed.
    int                m_ioBytes;

  protected:
    /// Destroy persistent object in the container
    virtual DbStatus destroyObject(ActionList::value_type&);
    /// Commit single entry to container
    virtual DbStatus writeObject(ActionList::value_type&);
  public:
    /// Standard constructor
    RootKeyContainer();
    /// Standard destructor
    virtual ~RootKeyContainer();
    /// Close the container and deallocate resources
    virtual DbStatus close();
    /// Open the container for object access
    virtual DbStatus open(const DbDatabase& dbH, 
                          const std::string& nam, 
                          const DbTypeInfo* info,
                          DbAccessMode mod);
    /// Ask if a given shape is supported
    virtual DbStatus isShapeSupported(const DbTypeInfo* /* typ */ ) const
    { return true;    }
    /// Define selection criteria
    virtual DbStatus select(DbSelect& criteria);
    /// Number of entries within the container
    virtual long long int size();
    /// Number of record in the container
    virtual long long int nextRecordId();
    /// Fetch next object address of the selection to set token
    virtual DbStatus fetch(DbSelect&      sel);
    /// Fetch a column identified by its link in the container
    virtual DbStatus fetch( const Token::OID_t& linkH, Token::OID_t& stmt);

    /// Find object by object identifier and load it into memory
   /** @param  ptr    [IN/OUT]  ROOT-style address of the pointer to object
      * @param  shape     [IN]   Object type
      * @param  oid      [OUT]   Object OID
      *
      * @return Status code indicating success or failure.
      */
    virtual DbStatus loadObject( void** ptr, ShapeH shape, 
                                 Token::OID_t& oid);

    /// Interface Implementation: Find entry in container
    virtual DbStatus load( void** ptr, ShapeH shape,
                           const Token::OID_t& linkH,
                           Token::OID_t& oid,
                           bool          any_next);

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

    /// Execute end of object modification requests during a transaction
    /** @param refTr    [IN]  Transaction reference
      *
      * @return DbStatus code indicating success or failure.  
      */
    /// Execute transaction action
    virtual DbStatus transAct(Transaction::Action action);
  };
}
#endif //POOL_ROOTKEYCONTAINER_H
