/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef APR_IFILECATALOG_H
#define APR_IFILECATALOG_H

#include "GaudiUtils/IFileCatalog.h"
#include "GaudiUtils/IFileCatalogMgr.h"
#include "GaudiKernel/SmartIF.h"

#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/IMessageSvc.h"

#include <iostream>

namespace pool {

   namespace FileCatalog {
      typedef std::string FileID;
   }
   
   class IFileCatalog : public AthMessaging  {
    public:

     typedef Gaudi::IFileCatalog::Files         Files;
     typedef Gaudi::IFileCatalog::Strings       Strings;
     typedef Gaudi::IFileCatalogMgr::Catalogs   Catalogs;
     
     
     IFileCatalog();
     ~IFileCatalog() {}
     
     // ------------------------------ Catalog interface  
     
     /// Create file identifier using UUID mechanism
     std::string createFID() const;

     void connect() { /* not doing anything in Gaudi FC */ }
     void disconnect() { /* not doing anything in Gaudi FC */ }
     /// Get the connect string
     const std::string& connectInfo() const { return m_fc->connectInfo(); }
       
     /// redirect to init() for Gaudi FC
     void start() { init(); }
     /// Parse the DOM tree of the XML catalog
     void init() { m_fc->init(); }
     /// Save catalog to file
     void commit() { m_fc->commit(); }
     /// Save catalog to file
     void rollback() { m_fc->rollback(); }
     /// Check if the catalog is read-only
     bool readOnly() const { return m_fc->readOnly(); }
     /// Check if the catalog should be updated
     bool dirty() const  { return m_fc->dirty(); }

     /// Get all FIDs 
     void getFIDs( Strings& fids ) const { m_fc->getFID(fids); }
     
     /// Get all logical names for a given FID. Return pairs <LFN,FID>
     void getLFNs( const std::string& fid, Files& files ) const { m_fc->getLFN(fid, files); }

     /// Dump all PFNames of the catalog and their attributes associate to the FileID
     void getPFNs( const std::string& fid, Files& files ) const { m_fc->getPFN(fid, files); }

     /// Get the first PFN + filetype for the given FID
     void getFirstPFN( const std::string& fid, std::string& pfn, std::string& tech ) const;

     /// Get FID and filetype for a given PFN
     void lookupFileByPFN( const std::string& pfn, std::string& fid, std::string& tech ) const;

     /// Return the status of a PFName
     bool existsPFN( const std::string& pfn ) const { return m_fc->existsPFN(pfn); }

     /// Lookup file identifier by physical file name
     std::string lookupPFN( const std::string& pfn ) const { return m_fc->lookupPFN(pfn); }

     /// Return the status of a LFName
     //    bool existsLFN( const std::string& lfn ) const = 0;

     /// Lookup file identified by logical file name
     void lookupFileByLFN( const std::string& lfn, std::string& fid ) const { fid = lookupLFN(lfn); }
     std::string lookupLFN( const std::string& lfn ) const { return m_fc->lookupLFN(lfn); }

     /// Return the status of a FileID
     //    bool existsFID( const std::string& fid ) const = 0;

     /// Delete FileID Node from the catalog
     void deleteFID( const std::string& FileID ) { m_fc->deleteFID(FileID); }

     /// Delete PFN from the catalog (delete entire FID entry if it was the last PFN)
     void deletePFN( const std::string& pfn ) { m_fc->deletePFN(pfn); }

     /// Register PFN, assign new FID if not given
     void registerPFN( const std::string& pfn, const std::string& ftype, std::string& fid );

     /// Rename PFN
     void renamePFN( const std::string& pfn, const std::string& newpfn ) { m_fc->renamePFN(pfn, newpfn); }

     /// adding replica to an existing PFN
     void addReplicaPFN( const std::string& pfn, const std::string& replica_pfn );
     
     /// adding replica to an existing FID
     void addReplicaFID( const std::string& fid,
                         const std::string& replica_pfn,
                         const std::string& replica_tech = "ROOT_All" );

     /// Create a Node for a FileID and DOM Node of the LFN with all the attributes
     // NOTE!  this method requires FID! (and not PFN)
     void registerLFN( const std::string& fid, const std::string& lfn ) const { m_fc->registerLFN(fid, lfn ); }


     // ------------------------- Catalog Manager interface

     /// Add new catalog identified by name to the existing ones
     void addReadCatalog( const std::string& connect ) { addCatalog(connect); }
     void addCatalog( const std::string& connect );

     /// Add new catalog identified by reference to the existing ones
     //void addCatalog(  Gaudi::IFileCatalog* cat )  { m_mgr->addCatalog(cat); }

     /// Remove catalog identified by name from the existing ones. * or '' removes all
     void removeCatalog( const std::string& connect )  { m_mgr->removeCatalog(connect); }
     
     /// Remove catalog identified by reference from the existing ones
     //void removeCatalog( const  Gaudi::IFileCatalog* cat )  { m_mgr->removeCatalog(cat); }
     /// Access catalog container
     Catalogs& catalogs() { return m_mgr->catalogs(); }
     /// Access catalog container (CONST)
     const Catalogs& catalogs() const  { return m_mgr->catalogs(); }
     /// Access to the (first) writable file catalog
     //Gaudi::IFileCatalog* writeCatalog( const std::string& fid = "" ) const  { return m_mgr->writeCatalog(fid); }
     /// Define the writable catalog identified by reference
     //void setWriteCatalog(  Gaudi::IFileCatalog* cat )  { m_mgr->setWriteCatalog(cat); }
     /// Define the writable catalog identified by name
     void setWriteCatalog( const std::string& connect );
     

  protected:
     SmartIF<Gaudi::IFileCatalogMgr>    m_mgr;
     SmartIF<Gaudi::IFileCatalog>       m_fc;
   };

}

#endif
