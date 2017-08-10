/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//
//        Package    : RootStorageSvc (The POOL project)
//
//        Author     : M.Frank
//====================================================================
#ifndef POOL_ROOTSTORAGESVC_ROOTDBASE_H
#define POOL_ROOTSTORAGESVC_ROOTDBASE_H 1

// Framework include files
#include "StorageSvc/DbDatabaseImp.h"

#include <set>
#include <map>
#include <mutex>

// Forward declarations
class TFile;
class TTree;
class IFileMgr;

/*
 * POOL namespace declaration
 */
namespace pool  {  

   class RootTreeContainer;
   
  /** @class RootDatabase RootDatabase.h src/RootDatabase.h
    *
    * Description:
    * ROOT specific implementation of Database file.
    *
    * @author  M.Frank
    * @date    1/8/2002
    * @version 1.0
    */
  class RootDatabase : public DbDatabaseImp  {
  public:
    enum { READ_COUNTER = 0, WRITE_COUNTER = 1, OTHER_COUNTER = 2 };
  private:
    /// Reference to the actual implemented file 
    TFile*        m_file;
    /// Persistency format version
    std::string   m_version;
    /// Counter statistics
    long long int m_counters[3];
    /// Default compression level
    int           m_defCompression;
    /// Default compressionalgorithm 
    int           m_defCompressionAlg;
    /// Default split level
    int           m_defSplitLevel;
    /// Default Autosave parameter for trees
    int           m_defAutoSave;
    /// Default buffer size parameter for Branches
    int           m_defBufferSize;
    /// Default policy mode for keyed objects
    int           m_defWritePolicy;
    /// Offset table length for branches
    int		  m_branchOffsetTabLen;
    /// Name of tree with cache
    std::string   m_treeNameWithCache;
    /// Default tree cache learn events
    int           m_defTreeCacheLearnEvents;

    /* ---  variables used with TREE_AUTO_FLUSH option for
            managing combined TTree::Fill for branch containers
    */
    // TTree names with requested AUTO_FLUSH value
    std::map< std::string, int >        m_autoFlushTrees;

    // mapping of opened (for update) branch containers, to their TTree
    typedef    std::set< RootTreeContainer* >   ContainerSet_t;
    std::map< TTree*, ContainerSet_t >  m_containersInTree;
    
    std::map< std::string, int >        m_customSplitLevel;
    
    IFileMgr*   m_fileMgr;

    // mutex to prevent concurrent read I/O from AuxDynReader
    std::mutex  m_iomutex;
    
  public:
    /// Standard Constuctor
    /** @param idb      [IN]  Pointer to implementation object
      *
      * @return Reference to initialized object
      */
    RootDatabase(IOODatabase* idb);

    /// Standard destructor
    virtual ~RootDatabase();

    /// Access to the actual implemented file 
    TFile* file()                             { return m_file;    }

    /// Access to the version string
    const std::string& fmtVersion() const     { return m_version; }

    /// Check for file-existence
    /** @param nam      [IN]  Name of the database to be checked.
      *
      * @return Boolean value indicating the database existence.
      */
    static bool exists(const std::string& nam);

    /// Access the size of the database: May be undefined for some technologies
    virtual long long int size()  const;

    /// Do some statistics: add number of bytes read/written/other
    void addByteCount(int which, long long int num_bytes);

    /// Do some statistics: retrieve number of bytes read/written/other
    long long int byteCount(int which) const;


    DbStatus    markBranchContainerForFill(RootTreeContainer*);
    
    void        registerBranchContainer(RootTreeContainer*);

    /// provide access to the I/O mutex for AuxDynReader and Containers
    std::mutex& ioMutex()               { return m_iomutex; }
    
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

    /// implementation of TREE_AUTO_FLUSH option - called from setOption()
    virtual DbStatus setAutoFlush(const DbOption& opt);

    /// Open Database object
    /** @param domH     [IN]  Handle to valid domain object
      *                       (validity ensured by upper levels).
      * @param nam      [IN]  Name of the database to be opened.
      * @param mode     [IN]  Desired session access mode.
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus open(const DbDomain&     domH, 
                          const std::string&  nam, 
                          DbAccessMode        mode);

    /// Re-open database with changing access permissions
    /** @param mode     [IN]  Desired session access mode.
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus reopen(DbAccessMode mode);

    /// Callback after successful open of a database object
    /** @param dbH      [IN]  Handle to valid database object
      * @param mode     [IN]  Desired session access mode.
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus onOpen(const DbDatabase& dbH, DbAccessMode      mode);

    /// Close database access
    /** @param mode     [IN]  Desired session access mode.
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus close(DbAccessMode mode);

    /// Start/Commit/Rollback Database Transaction
    virtual DbStatus transAct(DbTransaction& /* refTr */);

  };
}       // End namespace pool
#endif  /* POOL_ROOTSTORAGESVC_ROOTDBASE_H */
