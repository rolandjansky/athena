/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
     const std::string& connectInfo() const { return _fc->connectInfo(); }
       
     /// redirect to init() for Gaudi FC
     void start() { init(); }
     /// Parse the DOM tree of the XML catalog
     void init() { _fc->init(); }
     /// Save catalog to file
     void commit() { _fc->commit(); }
     /// Save catalog to file
     void rollback() { _fc->rollback(); }
     /// Check if the catalog is read-only
     bool readOnly() const { return _fc->readOnly(); }
     /// Check if the catalog should be updated
     bool dirty() const  { return _fc->dirty(); }

     /// Get all FIDs 
     void getFIDs( Strings& fids ) const { _fc->getFID(fids); }
     
     /// Get all logical names for a given FID. Return pairs <LFN,FID>
     void getLFNs( const std::string& fid, Files& files ) const { _fc->getLFN(fid, files); }

     /// Dump all PFNames of the catalog and their attributes associate to the FileID
     void getPFNs( const std::string& fid, Files& files ) const { _fc->getPFN(fid, files); }

     /// Get the first PFN + filetype for the given FID
     void getFirstPFN( const std::string& fid, std::string& pfn, std::string& tech ) const;

     /// Get FID and filetype for a given PFN
     void lookupFileByPFN( const std::string& pfn, std::string& fid, std::string& tech ) const;

     /// Return the status of a PFName
     bool existsPFN( const std::string& pfn ) const { return _fc->existsPFN(pfn); }

     /// Lookup file identifier by physical file name
     std::string lookupPFN( const std::string& pfn ) const { return _fc->lookupPFN(pfn); }

     /// Return the status of a LFName
     //    bool existsLFN( const std::string& lfn ) const = 0;

     /// Lookup file identified by logical file name
     void lookupFileByLFN( const std::string& lfn, std::string& fid ) const { fid = lookupLFN(lfn); }
     std::string lookupLFN( const std::string& lfn ) const { return _fc->lookupLFN(lfn); }

     /// Return the status of a FileID
     //    bool existsFID( const std::string& fid ) const = 0;

     /// Delete FileID Node from the catalog
     void deleteFID( const std::string& FileID ) { _fc->deleteFID(FileID); }

     /// Delete PFN from the catalog (delete entire FID entry if it was the last PFN)
     void deletePFN( const std::string& pfn ) { _fc->deletePFN(pfn); }

     /// Register PFN, assign new FID if not given
     void registerPFN( const std::string& pfn, const std::string& ftype, std::string& fid );

     /// Rename PFN
     void renamePFN( const std::string& pfn, const std::string& newpfn ) { _fc->renamePFN(pfn, newpfn); }

     /// adding replica to an existing PFN
     void addReplicaPFN( const std::string& pfn, const std::string& replica_pfn );
     
     /// adding replica to an existing FID
     void addReplicaFID( const std::string& fid,
                         const std::string& replica_pfn,
                         const std::string& replica_tech = "ROOT_All" );

     /// Create a Node for a FileID and DOM Node of the LFN with all the attributes
     // NOTE!  this method requires FID! (and not PFN)
     void registerLFN( const std::string& fid, const std::string& lfn ) const { _fc->registerLFN(fid, lfn ); }


     // ------------------------- Catalog Manager interface

     /// Add new catalog identified by name to the existing ones
     void addReadCatalog( const std::string& connect ) { addCatalog(connect); }
     void addCatalog( const std::string& connect );

     /// Add new catalog identified by reference to the existing ones
     //void addCatalog(  Gaudi::IFileCatalog* cat )  { _mgr->addCatalog(cat); }

     /// Remove catalog identified by name from the existing ones. * or '' removes all
     void removeCatalog( const std::string& connect )  { _mgr->removeCatalog(connect); }
     
     /// Remove catalog identified by reference from the existing ones
     //void removeCatalog( const  Gaudi::IFileCatalog* cat )  { _mgr->removeCatalog(cat); }
     /// Access catalog container
     Catalogs& catalogs() { return _mgr->catalogs(); }
     /// Access catalog container (CONST)
     const Catalogs& catalogs() const  { return _mgr->catalogs(); }
     /// Access to the (first) writable file catalog
     //Gaudi::IFileCatalog* writeCatalog( const std::string& fid = "" ) const  { return _mgr->writeCatalog(fid); }
     /// Define the writable catalog identified by reference
     //void setWriteCatalog(  Gaudi::IFileCatalog* cat )  { _mgr->setWriteCatalog(cat); }
     /// Define the writable catalog identified by name
     void setWriteCatalog( const std::string& connect );
     

  protected:
     SmartIF<Gaudi::IFileCatalogMgr>    _mgr;
     SmartIF<Gaudi::IFileCatalog>       _fc;
   };

}

#endif
