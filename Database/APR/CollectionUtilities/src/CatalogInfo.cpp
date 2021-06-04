/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CollectionUtilities/CatalogInfo.h"
#include "CollectionBase/ICollectionService.h"
#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/URIParser.h"
#include "CoralBase/MessageStream.h"

/**********************************************************

CatalogInfo is an concretization of a CmdLineArgs2
   to contain catalog info.

Qualifiers: -ccread, -fcread

**********************************************************/

#include <iostream>
using namespace std;
using namespace pool;

CatalogInfo::CatalogInfo( ) : CmdLineArgs2("CatalogInfo"),m_valid(false)
{
   QualList markers;
   markers.insert( std::make_pair("-usecatalog",    ArgQual(0)) );
   markers.insert( std::make_pair("-ccread",    ArgQual(1,true)) );
   markers.insert( std::make_pair("-fcread",    ArgQual(1,true)) );
   markers["-usecatalog"].desc << "flag of whether to use collection catalog (DEFAULT=false)";
   markers["-ccread"].desc << "read collection catalog connection string (DEFAULT = \"\")";
   markers["-fcread"].desc << "read file catalog connection string (DEFAULT = \"\")";
   setArgQuals(markers);
}


bool
CatalogInfo::evalArgs(std::vector<std::string>& argv)
{
   bool retc = true;
   retc = this->CmdLineArgs2::evalArgs(argv);
   if (!retc) return retc;

   if ( this->hasQual("-usecatalog") ) {
      m_valid=true;
   }

   if ( m_valid && this->hasQual("-ccread") ) {
      int ifirst = (*this)["-ccread"].first;
      int ilast  = (*this)["-ccread"].second;
      int i = ifirst;
      while ( i < ilast ) {
         m_collCatalogReadVec.push_back( argv[i] );
         ++i;
      }
   }

   if ( this->hasQual("-fcread") ) {
      int ifirst = (*this)["-fcread"].first;
      int ilast  = (*this)["-fcread"].second;
      int i = ifirst;
      while ( i < ilast ) {
         m_fileCatalogReadVec.push_back( argv[i] );
         ++i;
      }
   }
        
   return retc;

}


std::string
CatalogInfo::collCatalogN(unsigned int ind)
{            
   if ( ind < m_collCatalogReadVec.size() )
      return m_collCatalogReadVec[ind];
   else {  
      std::cerr << "Out of range request for collCatConnect" << std::endl;
      return std::string("BAD");
   }
}     


std::string
CatalogInfo::fileCatalogN(unsigned int ind)
{            
   if ( ind < m_fileCatalogReadVec.size() )
      return m_fileCatalogReadVec[ind];
   else {
      std::cerr << "Out of range request for fileCatConnect" << std::endl;
      return std::string("BAD");
   }
}     


void
CatalogInfo::setCatalogs ATLAS_NOT_THREAD_SAFE ( pool::ICollectionService *service )
{
   coral::MessageStream log( name() );
   for( size_t i=0; i<m_collCatalogReadVec.size(); i++ )    {
      service->addReadCatalog( m_collCatalogReadVec[i] );
      log << coral::Debug
	  << " adding collection catalog: " << m_collCatalogReadVec[i] << coral::MessageStream::endmsg;
   }
}
   


void
CatalogInfo::setFileCatalogs( pool::IFileCatalog *fc )
{
   coral::MessageStream log( name() );
   size_t en = m_fileCatalogReadVec.size();
   if( !en ) {
      pool::URIParser 		parser;
      parser.parse();
      fc->addReadCatalog(parser.contactstring()); 
   } else {
      for( size_t i=0; i<en; i++ )    {
	 fc->addReadCatalog( m_fileCatalogReadVec[i] );
	 log << coral::Debug
	     << " adding file catalog: " << m_fileCatalogReadVec[i] << coral::MessageStream::endmsg;
      }
   }
   fc->connect();
}
   





