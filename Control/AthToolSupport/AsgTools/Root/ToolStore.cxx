//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//

// System include(s):
#include <map>
#include <iostream>

// Local include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolStore.h"
#include "AsgTools/MsgStreamMacros.h"

// Create a simple version of the MSGSTREAM_REPORT_PREFIX macro for Athena:
#ifdef ASGTOOL_ATHENA
#   define MSGSTREAM_REPORT_PREFIX              \
   __FILE__ << ":" << __LINE__ << ": "
#endif // ASGTOOL_ATHENA

/// Helper macro for printing nicely formatted error messages
#define TOOLSTORE_ERROR( FNC, MSG )                                  \
   std::cout << FNC << " ERROR   " << MSGSTREAM_REPORT_PREFIX << MSG \
             << std::endl

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
         std::cout << "asg::ToolStore::put       WARNING Tool with name \""
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

#ifdef ASGTOOL_STANDALONE
      // Set the tool's name to the specified one:
      ptool->setName( name );
#endif // ASGTOOL_STANDALONE

      // Register the tool using the other function:
      return put( ptool );
   }

   IAsgTool* ToolStore::get( const std::string& name, bool silent ) {
      using namespace msgToolHandle;

      ToolMap_t::const_iterator itool = tools().find( name );
      if( itool != tools().end() )
        return itool->second;

      if (name.find ("ToolSvc.") != 0) {
        itool = tools().find( "ToolSvc." + name );
        if( itool != tools().end() )
          return itool->second;
      }

      if( ! silent ) {
        ANA_MSG_WARNING ("Tool with name \"" << name << "\" not found");
      }
      return nullptr;
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


#ifdef XAOD_STANDALONE
  void ToolStore::dumpToolConfig () {
    using namespace asg::msgProperty;

    // I am first putting everything into a map, so that the error
    // messages don't interleave the property values
    std::map<std::string,std::map<std::string,std::string> > properties;

    for (auto& tool : tools())
    {
      auto& myproperties = properties[tool.first];
      myproperties[""] = std::string (typeid(*tool.second).name()) + "/" + tool.first;
      AsgTool *mytool = dynamic_cast<AsgTool*>(tool.second);
      if (mytool == nullptr)
      {
	ANA_MSG_ERROR ("tool " << tool.first << " not of AsgTool type");
	myproperties[""] += " <invalid type>";
      } else
      {
	for (auto& property : mytool->getPropertyMgr()->getProperties())
	{
	  std::string asString;
	  if (property.second->getString (asString).isFailure())
	  {
	    ANA_MSG_ERROR ("on property " << property.first << " for tool " << tool.first);
	    myproperties[property.first] = "<<invalid>>";
	  } else
	    myproperties[property.first] = asString;
	}
      }
    }
    for (const auto& myproperties : properties)
    {
      for (auto& property : myproperties.second)
      {
	std::cout << myproperties.first;
	if (!property.first.empty())
	  std::cout << "." << property.first;
	std::cout << " = " << property.second << "\n";
      }
    }
    std::cout << std::flush;
  }
#endif

} // namespace asg
