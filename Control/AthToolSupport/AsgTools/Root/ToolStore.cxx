// $Id: ToolStore.cxx 620407 2014-10-07 13:40:28Z krasznaa $

// System include(s):
#include <map>
#include <iostream>

// Local include(s):
#include "AsgTools/ToolStore.h"
#include "AsgTools/MsgStreamMacros.h"

/// Helper macro for printing nicely formatted error messages
#define TOOLSTORE_ERROR( FNC, MSG )             \
   std::cerr << FNC << " ERROR " << MSGSTREAM_REPORT_PREFIX << MSG << std::endl

/// Convenience type definition
typedef std::map< std::string, asg::IAsgTool* > ToolMap_t;

namespace {

   /// Helper function providing the application-wide tool registry
   ToolMap_t& tools() {

      static ToolMap_t s_tools;
      return s_tools;
   }

} // private namespace

namespace asg {

   StatusCode ToolStore::put( IAsgTool* ptool ) {

      // Start with a little sanity check:
      if( ! ptool ) {
         TOOLSTORE_ERROR( "asg::ToolStore::put      ",
                          "Received a null pointer" );
         return StatusCode::FAILURE;
      }

      // Get and check the name of the tool:
      const std::string& name = ptool->name();
      if( ! name.size() ) {
         TOOLSTORE_ERROR( "asg::ToolStore::put      ",
                          "The received tool doesn't have a name" );
         return StatusCode::FAILURE;
      }

      // Check whether we already have a tool with this name:
      if( tools().find( name ) != tools().end() ) {
         std::cout << "asg::ToolStore::put     WARNING Tool with name \""
                   << name << "\" already registered" << std::endl;
         return StatusCode::FAILURE;
      }

      // Remember the tool:
      tools()[ name ] = ptool;
      return StatusCode::SUCCESS;
   }

   StatusCode ToolStore::put( IAsgTool* ptool, const std::string& name ) {

      // Start with a little sanity check:
      if( ! ptool ) {
         TOOLSTORE_ERROR( "asg::ToolStore::put      ",
                          "Received a null pointer" );
         return StatusCode::FAILURE;
      }

      // Check the provided name:
      if( ! name.size() ) {
         TOOLSTORE_ERROR( "asg::ToolStore::put      ",
                          "Received an empty name" );
      }

      // Set the tool's name to the specified one:
      ptool->setName( name );

      // Register the tool using the other function:
      return put( ptool );
   }

   IAsgTool* ToolStore::get( const std::string& name ) {

      ToolMap_t::const_iterator itool = tools().find( name );
      if( itool == tools().end() ) {
         std::cout << "asg::ToolStore::get     WARNING Tool with name \""
                   << name << "\" not found" << std::endl;
         return 0;
      }
      return itool->second;
   }

   StatusCode ToolStore::remove( const IAsgTool* tool ) {

      // Delegate the call to the other function:
      return remove( tool->name() );
   }

   StatusCode ToolStore::remove( const std::string& name ) {

      // Remove the tool, not checking if the call was successful or not:
      tools().erase( name );
      return StatusCode::SUCCESS;
   }

} // namespace asg
