/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiUtils/IFileCatalog.h"
#include "GaudiUtils/IFileCatalogMgr.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

#include <algorithm>
#include "POOLCore/SystemTools.h"
#include "POOLCore/Exception.h"
#include "FileCatalog/URIParser.h"
#include "FileCatalog/IFileCatalog.h"

#include "AthenaBaseComps/AthMessaging.h"

#include <iostream>

using namespace pool;

pool::IFileCatalog::IFileCatalog()
  : AthMessaging( Gaudi::svcLocator()->service<IMessageSvc>("MessageSvc").get(), "APRFileCatalog" )
{
   _mgr = Gaudi::svcLocator()->service<Gaudi::IFileCatalogMgr>( "Gaudi::MultiFileCatalog" );
   _fc = _mgr;
   setLevel( SystemTools::GetOutputLvl() );
}
     

void pool::IFileCatalog::
getFirstPFN( const std::string& fid, std::string& pfn, std::string& tech ) const
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
void pool::IFileCatalog::
lookupFileByPFN( const std::string& pfn, std::string& fid, std::string& tech ) const
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
   

/// Register PFN, assign new FID if not given
void pool::IFileCatalog::
registerPFN( const std::string& pfn, const std::string& ftype, std::string& fid )
{
   if( existsPFN(pfn) ) {
      throw pool::Exception(std::string("PFN '") + pfn + "' already registered", "registerPFN", "FileCatalog");
   }
//   std::cout << "msg() level" << msg().level() << std::endl;
   ATH_MSG_DEBUG("Registering PFN=" << pfn << " of type=" << ftype << " GUID=" << fid);
   if( fid.empty() ) fid = createFID();
   _fc->registerPFN(fid, pfn, ftype);
}


/// adding replica to a PFN
void pool::IFileCatalog::
addReplicaPFN( const std::string& pfn, const std::string& replica_pfn )
{ 
   std::string fid = _fc->lookupPFN(pfn);
   if( fid.empty() )
      throw pool::Exception(std::string("PFN '") + pfn + "' not found", "addReplicaPFN", "FileCatalog");
   // find the filetype for the PFN being replicated
   Files   pfns;
   getPFNs( fid, pfns );
   for( const auto& pfns_entry: pfns ) {
      if( pfns_entry.first == pfn ) {
         _fc->registerPFN(fid, replica_pfn, pfns_entry.second);
         return;
      }
   }
}
     
/// adding replica to a FID
void pool::IFileCatalog::
addReplicaFID( const std::string& fid, const std::string& replica_pfn, const std::string& replica_tech )
{ 
   if( fid.empty() )
      throw pool::Exception(std::string("FID not specified"), "addReplicaFID", "FileCatalog");
   _fc->registerPFN(fid, replica_pfn, replica_tech);
}


// ------------------------- Catalog Manager interface


/// Add new catalog identified by name to the existing ones
void pool::IFileCatalog::addCatalog( const std::string& connect )
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

/// Define the writable catalog identified by name
void pool::IFileCatalog::setWriteCatalog( const std::string& connect )
{
   URIParser p(connect);
   p.parse();
   addCatalog( connect );
   _mgr->setWriteCatalog( _mgr->findCatalog( p.url(), true ) );
}
     

