/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EntityResolver.cxx 474270 2011-12-16 15:09:49Z krasznaa $

// System include(s):
#include <cstdlib>

// STL include(s):
#include <string>
#include <iostream>

// Boost include(s):
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/filesystem.hpp>

// Xerces-c include(s):
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/util/XMLString.hpp>

// Local include(s):
#include "TrigConfMuctpi/EntityResolver.h"
#include "Logging.h"

//
// While it's unlikely that the code will be used on Windows, it's a simple
// trick to make it work there. So why not...
//
#ifdef _WIN32
static const char* PATH_SEPARATOR = ",;";
#else
static const char* PATH_SEPARATOR = ",:";
#endif

namespace {

   /// Function finding the actual file name
   /**
    * I basically just stole this implementation from Gaudi, as GaudiKernel
    * is unfortunately not available for this package...
    *
    * @param logicalFileName The file that we want to find
    * @param searchPath The search path in which we're looking for the file
    * @returns The full file name if found, or "" if not
    */
   std::string findFile( const boost::filesystem::path& logicalFileName,
                         const std::string& searchPath ) {

      // Check if this is the full file name already:
      if( boost::filesystem::is_regular_file( logicalFileName ) ) {
         return boost::filesystem::absolute( logicalFileName ).string();
      }

      // Search for it in "./" just to be sure:
      boost::filesystem::path local = boost::filesystem::initial_path() / logicalFileName;
      if( boost::filesystem::is_regular_file( local ) ) {
         return boost::filesystem::absolute( local ).string();
      }

      // Now look for the file in the search path:
      std::vector< std::string > splitPath;
      boost::algorithm::split( splitPath, searchPath,
                               boost::is_any_of( PATH_SEPARATOR ),
                               boost::token_compress_on );
      std::vector< std::string >::const_iterator dir_itr = splitPath.begin();
      std::vector< std::string >::const_iterator dir_end = splitPath.end();
      for( ; dir_itr != dir_end; ++dir_itr ) {
         boost::filesystem::path filePath = *dir_itr / logicalFileName;
         if( boost::filesystem::is_regular_file( filePath ) ) {
            return boost::filesystem::absolute( filePath ).string();
         }
      }

      // If we got this far, we didn't manage to find the file...
      return "";
   }

} // namespace

namespace Muctpi {

   /// Prefix for the XML and DTD files
   static const std::string PACKAGE_PREFIX = "TrigConfMuctpi/";

   /**
    * This function is used to find the full path names of the MuCTPI LUT
    * XML and DTD files.
    *
    * @param publicId Not used by the code
    * @param systemId The logical file name (without the absolute path)
    * @returns The XercesC object that can read in the file, or a null pointer
    *          if the file could not be found
    */
   xercesc::InputSource* EntityResolver::resolveEntity( const XMLCh* const,
                                                        const XMLCh* const systemId ) {

      // Make an std::string out of the DTD name:
      char* cname = xercesc::XMLString::transcode( systemId );
      std::string entity( cname );
      xercesc::XMLString::release( &cname );

      // A small sanity check:
      if( ! entity.length() ) return 0;

      // Get the full path name:
      std::string fullname = findFile( PACKAGE_PREFIX + entity,
                                       ::getenv( "XMLPATH" ) );

      // Another sanity check:
      if( ! fullname.length() ) {
         REPORT_ERROR( "Muctpi::EntityResolver",
                       "Couldn't find entity with name \"" << entity
                       << "\" in XMLPATH" );
         return 0;
      }

      // Tell the user the results:
      REPORT_INFO( "Muctpi::EntityResolver",
                   entity << " --> " << fullname );

      // Create the file reader object for XercesC:
      XMLCh* xname = xercesc::XMLString::transcode( fullname.c_str() );
      xercesc::LocalFileInputSource* result = new xercesc::LocalFileInputSource( xname );
      xercesc::XMLString::release( &xname );

      return result;
   }

} // namespace Muctpi
