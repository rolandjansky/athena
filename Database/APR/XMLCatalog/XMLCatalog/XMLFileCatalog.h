/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOL_XMLFILECATALOG_H
#define POOL_XMLFILECATALOG_H
#include "FileCatalog/FCImpl.h"
#include "FileCatalog/FCBuf.h"

#ifdef HAVE_GAUDI_PLUGINSVC
#include "Gaudi/PluginService.h"
#endif
#include "GAUDI_VERSION.h"

namespace pool { 

/**
 *        @class XMLFileCatalog XMLFileCatalog.h XMLCatalog/XMLFileCatalog.h 
 *          This class is the concrete implementation of the 
 *          POOL XML based File Catalog 
 *
 *       @Author: Maria Girone
 */

class PoolXMLFileCatalog;
class XMLMetaDataCatalog; 
class XMLFileCatalog: public FCImpl{
 public:
#ifdef HAVE_GAUDI_PLUGINSVC
  #if GAUDI_VERSION > CALC_GAUDI_VERSION(25, 3) 
    typedef Gaudi::PluginService::Factory<FCImpl*> Factory;
  #else  
    typedef Gaudi::PluginService::Factory0<FCImpl*> Factory;
  #endif
#endif
  /**Default constructor.
   */
  XMLFileCatalog();

  /**virtual destructor.
   */
  virtual ~XMLFileCatalog();

  //
  //Connection and transaction control operations.
  //   

  /** XMLFileCatalog connection method.
      @param url [IN] std::string standard URL of the XML catalog.
  */
  virtual void connect();

  /** Close the connection to the XMLFileCatalog.
   */
  virtual void disconnect() const;
  /** Start the catalog transaction.
   */
  virtual void start() const;
  /** Commit the catalog transaction.
      @param cm [IN] catalog commit mode.
      REFRESH mode: catalog will be reinitialised on next transaction start.
      ONHOLD mode: catalog will not be reinitialised on next transaction start.
   */
  virtual void commit(FileCatalog::CommitMode cm=FileCatalog::REFRESH) const;

  /**Rollback the catalog transaction.
   */
  virtual void rollback() const;  

  //
  //File registration operations
  //
  
  /**Register a PFN when a new file is created, 
     returns the corresponding std::string 
     @param  pfn [IN] the Physical file name
     @param  filetype [IN] the filetype pf the PFN
     @param  fid [OUT] Guid of the file
  */
  virtual void registerPFN(const std::string& pfn, 
			   const std::string& filetype,
			   FileCatalog::FileID& fid ) const;   

  /** Register LFN 
      @param pfn PFN
      @param lfn LFN
  */
  virtual void registerLFN(const std::string& pfn, 
			   const std::string& lfn) const;


 /** Add a replica file name in the catalog.
     @param pfn [IN] the PFN of the master copy file.
     @param rpf [IN] the PFN of the replica to be added in the catalog. 
  */
  virtual void addReplicaPFN(const std::string& pfn, 
			     const std::string& rpf) const;

 /** Add a mapping in the catalog.
     @param guid [IN] the Guid of the file.
     @param pf   [IN] the PFN of the file to be added in the catalog. 
  */
  virtual void addPFNtoGuid(const FileCatalog::FileID& guid, 
			    const std::string& pf, 
			    const std::string& filetype) const;

  /** Rename a PFN in the catalog. The use case is the file is moved.
      @param pfn [IN] old PFN.
      @param newpf [IN] new PFN.
  */
  virtual void renamePFN(const std::string& pfn, 
			 const std::string& newpfn) const;
  
  /** Lookup the fileID with given PFN.
      @param pfn [IN] PFN.
      @param fid [OUT] FileID, return empty string if not found.
      @param ftype [OUT] file type , return empty string if not found.
   */
  virtual void lookupFileByPFN(const std::string& pfn, 
			       FileCatalog::FileID& fid, 
			       std::string& ftype) const;
  
  /** Lookup the FileID with given LFN.
      @param lfn [IN] LFN.
      @param fid [OUT] FileID.
   */
  virtual void lookupFileByLFN(const std::string& lfn, 
			       FileCatalog::FileID& fid) const;

  /** Lookup the PFN of a file with given std::string in the catalog.
      Throws exception when the file is nonexistant.
      @param fid [IN] FileID
      @param omode [IN] FileOpenMode(read,write,update).
      @param amode [IN] FileAccessPattern(sequential,random,partial random).
      A hint to decide on how to ship and/or replicate the file. 
      @param pfn [OUT] PFN.
      @param filetype [OUT] file type.
   */
  virtual void lookupBestPFN(const FileCatalog::FileID& fid, 
			     const FileCatalog::FileOpenMode& omode,
			     const FileCatalog::FileAccessPattern& amode,
			     std::string& pfn,
			     std::string& filetype) const; 

  virtual void insertPFN(PFNEntry& pentry) const;
  virtual void insertLFN(LFNEntry& lentry) const;
  virtual void deletePFN(const std::string& pfn) const;
  virtual void deleteLFN(const std::string& lfn) const;
  virtual void deleteEntry(const FileCatalog::FileID& guid) const;

  virtual bool isReadOnly() const;
 
  virtual bool retrievePFN(const std::string& query, 
			   FCBuf<PFNEntry>& buf, 
			   const size_t& start );
  virtual bool retrieveLFN(const std::string& query, 
			   FCBuf<LFNEntry>& buf, 
			   const size_t& start );
  virtual bool retrieveGuid(const std::string& query, 
			     FCBuf<FileCatalog::FileID>& buf, 
			     const size_t& start );
  
 private:
  friend class XMLMetaDataCatalog;

 private:
  void ClassHandle() const;
  static PoolXMLFileCatalog* castConnectionHandle(PoolXMLFileCatalog* con);

 private:
  mutable PoolXMLFileCatalog* XMLcon;
  mutable bool is_started;
  mutable bool to_be_updated;
  mutable bool dontstart;
    
  
};
} /* ns pool */

#endif
