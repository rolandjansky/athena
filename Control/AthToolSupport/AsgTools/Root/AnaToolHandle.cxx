//        Copyright Iowa State University 2015.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <AsgTools/AnaToolHandle.h>

#include "AsgTools/ToolStore.h"

#ifdef ROOTCORE
#include <TInterpreter.h>
#include <TROOT.h>
#else
#include <GaudiKernel/AlgTool.h>
#include <AsgTools/MessageCheck.h>
#include <GaudiKernel/IToolSvc.h>
#include <GaudiKernel/IJobOptionsSvc.h>
#endif

//
// method implementations
//

#ifdef ROOTCORE

namespace asg
{
  namespace detail
  {
    StatusCode makeToolRootCore (const std::string& type,
				 const std::string& name,
				 AsgTool*& tool)
    {
      using namespace msgToolHandle;

      ATH_MSG_DEBUG("Creating tool of type " << type);
	
      // Load the ROOT dictionary
      TClass* toolClass = TClass::GetClass( type.c_str() );
      if(!toolClass){
	ATH_MSG_ERROR("Unable to load class dictionary for type " << type);
	return StatusCode::FAILURE;
      }
	
      // In order to call a non-default constructor with the ROOT dictionary,
      // we have to perform some low-level ROOT interpreter magic. First, we
      // need the name of the constructor, which we get by stripping off the
      // namespace prefix.
      std::string constructorName = type.substr( type.rfind(":") + 1 );
	
      // Next, we prepare the constructor function.
      auto callFunc = gInterpreter->CallFunc_Factory();
      long offset = 0; // not relevant for ctors
      const char* argForm = "const std::string&";
      gInterpreter->CallFunc_SetFuncProto(callFunc, toolClass->GetClassInfo(),
					  constructorName.c_str(), argForm,
					  &offset);
	
      // Make sure the function is well-formed
      if(!gInterpreter->CallFunc_IsValid(callFunc)) {
	ATH_MSG_ERROR("Invalid constructor call for type " << type <<
		      " and constructor " << constructorName);
	gInterpreter->CallFunc_Delete(callFunc);
	return StatusCode::FAILURE;
      }
	
      // Set the name constructor argument
      gInterpreter->CallFunc_SetArg(callFunc, &name);
	
      // Call the function. Here it's a little strange, because the ROOT function
      // returns a 'long', so we have to cast it back to a useful pointer.
      tool = reinterpret_cast<asg::AsgTool*>
	( gInterpreter->CallFunc_ExecInt(callFunc, 0));
	
      // Cleanup the CallFunc
      gInterpreter->CallFunc_Delete(callFunc);
	
      // Construct tool from dictionary with TClass::New.
      // Doesn't work. Requires a default constructor.
      ////asg::AsgTool* tool = static_cast<asg::AsgTool*>( toolClass->New() );
      ////tool->setName(name);
	
      return StatusCode::SUCCESS;
    }
  }
}

#else

namespace asg
{
  namespace detail
  {
   StatusCode hasPropertiesInCatalogue( const std::string& toolName ) {
      ServiceHandle<IJobOptionsSvc> svc("JobOptionsSvc","AnaToolHandle");
      if( svc.retrieve().isFailure() ) return false;
      auto props = svc->getProperties(toolName);
      StatusCode out = StatusCode::FAILURE;
      if( props && props->size()>0 ) out = StatusCode::SUCCESS;
      svc.release().ignore();
      //delete props;
      return out;
   }

    StatusCode addPropertyToCatalogue( const std::string& toolName, const std::string& propertyName, const std::string& propertyValue ) {
//std::cout << "Adding " << toolName << " ." << propertyName << " = " << propertyValue << std::endl;
      ServiceHandle<IJobOptionsSvc> joSvc("JobOptionsSvc","AnaToolHandle");
      //check if propertyName contains '.' . If it does, then assume settng a property of a private tool, so adjust toolname
      std::string theToolName = toolName; std::string thePropertyName=propertyName;
      std::string::size_type dotLocation = thePropertyName.find_last_of('.');
      if(dotLocation != std::string::npos) {
         theToolName = toolName + "." + thePropertyName.substr(0,dotLocation);
         thePropertyName = thePropertyName.substr(dotLocation+1,thePropertyName.length()-dotLocation);
      }
      if(joSvc->addPropertyToCatalogue( theToolName , StringProperty(thePropertyName,propertyValue) ).isFailure()) return StatusCode::FAILURE;
      if(joSvc.release().isFailure()) return StatusCode::FAILURE;
      return StatusCode::SUCCESS;
   }



   StatusCode removePropertyFromCatalogue( const std::string& toolName, const std::string& propertyName ) {
      ServiceHandle<IJobOptionsSvc> joSvc("JobOptionsSvc","AnaToolHandle");
      //check if propertyName contains '.' . If it does, then assume settng a property of a private tool, so adjust toolname
      std::string theToolName = toolName; std::string thePropertyName=propertyName;
      std::string::size_type dotLocation = thePropertyName.find_last_of('.');
      if(dotLocation != std::string::npos) {
         theToolName = toolName + "." + thePropertyName.substr(0,dotLocation);
         thePropertyName = thePropertyName.substr(dotLocation+1,thePropertyName.length()-dotLocation);
      }
      if(joSvc->removePropertyFromCatalogue( theToolName , thePropertyName ).isFailure()) return StatusCode::FAILURE;
      if(joSvc.release().isFailure()) return StatusCode::FAILURE;
      return StatusCode::SUCCESS;
   }
    StatusCode toolExists( const std::string& fullName, interfaceType_t*& tool ) {
      ServiceHandle<IToolSvc> toolSvc("ToolSvc","AnaToolHandle");
      toolSvc.retrieve().ignore();
      auto tools = toolSvc->getTools();
      bool out(false);
      for(auto atool : tools) {
         if(atool->name() == fullName) {out=true;tool=atool;break;}
      }
      toolSvc.release().ignore();
      return out;
   }

   StatusCode factoryExists( const std::string& type ) {
      using Gaudi::PluginService::Details::Registry;
      Registry &reg = Registry::instance();
      if(reg.getInfo(type).library=="unknown") return StatusCode::FAILURE;
      return StatusCode::SUCCESS;
   }

   StatusCode copyPropertiesInCatalogue( const std::string& fromTool, const std::string& toTool ) {
      using namespace msgToolHandle;
      //this method copies any properties assigned to 'fromTool' to 'toTool'
      //purpose of this is because python joboptions will set joboptions properties using the name of the handle
      //but we would like the name of the property (in declareProperty) to be used for anatoolhandles instead, in the case of private tools
//std::cout << "copy : " << fromTool << " -> " << toTool << std::endl;
      if(fromTool == toTool) return StatusCode::SUCCESS; //nothing to do

      ServiceHandle<IJobOptionsSvc> joSvc("JobOptionsSvc","AnaToolHandle");
      auto fromProps = joSvc->getProperties(fromTool);
      if(fromProps) {
         for(auto prop : *fromProps) {
            ANA_CHECK( joSvc->addPropertyToCatalogue( toTool , *prop ) );
         }
         for(auto prop : *fromProps) {
            ANA_CHECK( joSvc->removePropertyFromCatalogue( fromTool , prop->name() ) );
         }
      }
      //now also check for subtools which will need copying over 
      auto clients = joSvc->getClients();
      for(auto& client : clients) {
         if(client.find(fromTool+".") == 0 && client!=fromTool) {
            std::string newClient(client); newClient.replace(0,strlen(fromTool.c_str()),toTool);
            ANA_CHECK( copyPropertiesInCatalogue( client , newClient ) );
         }
      }
      if(joSvc.release().isFailure()) return StatusCode::FAILURE;
      return StatusCode::SUCCESS;
   }


  }


}

#endif
