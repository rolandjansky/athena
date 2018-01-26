/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOLCORE_IFILECATALOG_H
#define POOLCORE_IFILECATALOG_H

#include "GaudiUtils/IFileCatalog.h"
#include "GaudiUtils/IFileCatalogMgr.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

#include <algorithm>
#include "POOLCore/Exception.h"
#include "POOLCore/URIParser.h"

#include "POOLCore/SystemTools.h"
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
     typedef Gaudi::IFileCatalogMgr::Catalogs   Catalogs;
     
     
     IFileCatalog() : AthMessaging( Gaudi::svcLocator()->service<IMessageSvc>("MessageSvc").get(), "APRFileCatalog" )
     {
        Gaudi::svcLocator()->service<IMessageSvc>("MessageSvc")->setOutputLevel("XMLCatalog",  MSG::WARNING );
        _mgr = Gaudi::svcLocator()->service<Gaudi::IFileCatalogMgr>( "Gaudi::MultiFileCatalog" );
        _fc = _mgr;
        msg().setLevel( SystemTools::GetOutputLvl() );
        //Gaudi::svcLocator()->service<IMessageSvc>("MessageSvc")->setOutputLevel("XMLCatalog",  SystemTools::GetOutputLvl() );
     }
     
     ~IFileCatalog() {}
     
     // --- Catalog interface  
     
     /// Create file identifier using UUID mechanism
     std::string createFID() const { return _fc->createFID(); }
     /// Access to connect string
//    const std::string& connectInfo() const = 0;

     void connect() { /* not doing anything in Gaudi FC */ }
     void disconnect() { /* not doing anything in Gaudi FC */ }
     
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

     /// Dump all PFNames of the catalog and their attributes associate to the FileID
     void getPFNs( const std::string& fid, Files& files ) const { return _fc->getPFN(fid, files); }

     /// Get the first PFN + filetype for the given FID
     void getFirstPFN( const std::string& fid, std::string& pfn, std::string& tech ) const
     {
        Files   pfns_techs;
        getPFNs( fid, pfns_techs );
        if( pfns_techs.empty() ) {
           pfn.clear();
           tech.clear();
        } else {
           pfn = pfns_techs[0].first;
           tech = pfns_techs[0].second;
        }
     }

     /// Get FID and filetype for a given PFN
     void lookupFileByPFN( const std::string& pfn, std::string& fid, std::string& tech ) const
     {
        fid = lookupPFN( pfn );
        if( !fid.empty() ) {
           Files   pfns_techs;
           getPFNs( fid, pfns_techs );
           tech.clear();
           for( const auto& attr_pair: pfns_techs ) {
              if( attr_pair.first == pfn )
                 tech = attr_pair.second;
           }
        }
     }
   
     /// Dump all LFNames of the catalog associate to the FileID
//    void getLFN( const std::string& fid, Files& files ) const = 0;
     /// Dump all file Identifiers
//    void getFID( Strings& fids ) const = 0;
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
     void deleteFID( const std::string& FileID ) { return _fc->deleteFID(FileID); }

     /// Register PFN, assign new FID if not given
     void registerPFN( const std::string& pfn, const std::string& ftype, std::string& fid )
     {
        if( existsPFN(pfn) ) {
           throw pool::Exception(std::string("PFN '") + pfn + "' already registered", "registerPFN", "FileCatalog");
        } 
        if( fid.empty() ) fid = createFID();
        _fc->registerPFN(fid, pfn, ftype);
     }

     /// adding replica
     void addReplicaPFN( const std::string& pfn, const std::string& replica_pfn )
     { 
        std::string fid = _fc->lookupPFN(pfn);
        if( fid.empty() )
           throw pool::Exception(std::string("PFN '") + pfn + "' not found", "addReplicaPFN", "FileCatalog");
        Files   pfns;
        getPFNs( fid, pfns );
        for( const auto& pfns_entry: pfns ) {
           if( pfns_entry.first == pfn ) {
              _fc->registerPFN(fid, replica_pfn, pfns_entry.second);
           }
        }
     }
     
     /// Create a Node for a FileID and DOM Node of the LFN with all the attributes
     // NOTE!  this method requires FID! (and not PFN)
     void registerLFN( const std::string& fid, const std::string& lfn ) const { _fc->registerLFN(fid, lfn ); }
     /// Create a Node for a FileID and DOM Node
//    void registerFID( const std::string& fid ) const = 0;


     // ------------------------- Catalog Manager interface

     /// Find catalog by connect string
     Gaudi::IFileCatalog* findCatalog( const std::string& connect, bool must_be_writable ) const { return _mgr->findCatalog(connect, must_be_writable); }

     /// Add new catalog identified by name to the existing ones
     void addReadCatalog( const std::string& connect ) { addCatalog(connect); }

     void addCatalog( const std::string& connect )
     {
        URIParser p(connect);
        p.parse();
        Catalogs& cats = _mgr->catalogs();     
        auto i = std::find_if( cats.begin(), cats.end(),
                               [&]( const Gaudi::IFileCatalog* f )
                               { return p.url() == f->connectInfo(); } );
        if( i==cats.end() ) {
           // add a new catalog
           _mgr->addCatalog(connect);
        }
     }

     /// Add new catalog identified by reference to the existing ones
     void addCatalog(  Gaudi::IFileCatalog* cat )  { _mgr->addCatalog(cat); }
     /// Remove catalog identified by name from the existing ones
     void removeCatalog( const std::string& connect )  { _mgr->removeCatalog(connect); }
     /// Remove catalog identified by reference from the existing ones
     void removeCatalog( const  Gaudi::IFileCatalog* cat )  { _mgr->removeCatalog(cat); }
     /// Access catalog container
     Catalogs& catalogs() { return _mgr->catalogs(); }
     /// Access catalog container (CONST)
     const Catalogs& catalogs() const  { return _mgr->catalogs(); }
     /// Access to the (first) writable file catalog
     Gaudi::IFileCatalog* writeCatalog( const std::string& fid = "" ) const  { return _mgr->writeCatalog(fid); }
     /// Define the writable catalog identified by reference
     void setWriteCatalog(  Gaudi::IFileCatalog* cat )  { _mgr->setWriteCatalog(cat); }
     /// Define the writable catalog identified by name
     void setWriteCatalog( const std::string& connect )
     {
        // std::cout << " setWriteCatalog: " << connect << std::endl;
        URIParser p(connect);
        p.parse();
        addCatalog( connect );
        setWriteCatalog( findCatalog(p.url(), true) );
     }
     

  protected:
     SmartIF<Gaudi::IFileCatalogMgr>    _mgr;
     SmartIF<Gaudi::IFileCatalog>       _fc;
   };

}

#endif
