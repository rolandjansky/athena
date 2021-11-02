/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <map>
#include <iostream>

// Local include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolStore.h"
#include "AsgTools/MessageCheckAsgTools.h"
#include "AsgMessaging/MsgStreamMacros.h"

/// Convenience type definition
typedef std::map< std::string, asg::IAsgTool* > ToolMap_t;

namespace {

   /// The application-wide tool registry
   static ToolMap_t s_tools ATLAS_THREAD_SAFE;
  std::mutex s_toolMutex;

} // private namespace

namespace asg {

   StatusCode ToolStore::put( IAsgTool* ptool ) {
      using namespace msgToolStore;

      // Start with a little sanity check:
      if( ! ptool ) {
         ANA_MSG_ERROR( "asg::ToolStore::put: Received a null pointer" );
         return StatusCode::FAILURE;
      }

      // Get and check the name of the tool:
      const std::string& name = ptool->name();
      if( ! name.size() ) {
         ANA_MSG_ERROR( "asg::ToolStore::put: The received tool doesn't have a name" );
         return StatusCode::FAILURE;
      }

      std::lock_guard<std::mutex> lock (s_toolMutex);
      // Check whether we already have a tool with this name:
      if( s_tools.find( name ) != s_tools.end() ) {
         ANA_MSG_WARNING ("asg::ToolStore::put: Tool with name \""
                        << name << "\" already registered");
         return StatusCode::FAILURE;
      }

      // Remember the tool:
      s_tools[ name ] = ptool;
      return StatusCode::SUCCESS;
   }

   IAsgTool* ToolStore::get( const std::string& name, bool silent ) {
      using namespace msgToolStore;

      std::lock_guard<std::mutex> lock (s_toolMutex);
      ToolMap_t::const_iterator itool = s_tools.find( name );
      if( itool != s_tools.end() )
        return itool->second;

      if (name.compare(0, 8, "ToolSvc.") != 0) {
        itool = s_tools.find( "ToolSvc." + name );
        if( itool != s_tools.end() )
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

      std::lock_guard<std::mutex> lock (s_toolMutex);
      // Remove the tool, not checking if the call was successful or not:
      s_tools.erase( name );
      return StatusCode::SUCCESS;
   }


#ifdef XAOD_STANDALONE
  void ToolStore::dumpToolConfig () {
    using namespace asg::msgProperty;

    // I am first putting everything into a map, so that the error
    // messages don't interleave the property values
    std::map<std::string,std::map<std::string,std::string> > properties;

    std::lock_guard<std::mutex> lock (s_toolMutex);
    for (auto& tool : s_tools)
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
