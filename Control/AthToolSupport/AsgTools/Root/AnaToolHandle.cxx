/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgTools/AnaToolHandle.h>
#include "CxxUtils/checker_macros.h"
#include <mutex>

#include "AsgTools/ToolStore.h"

#ifdef XAOD_STANDALONE
#include <TInterpreter.h>
#include <TROOT.h>
#include <regex>
#else
#include <GaudiKernel/AlgTool.h>
#include <AsgMessaging/MessageCheck.h>
#include <GaudiKernel/IToolSvc.h>
#include "Gaudi/Interfaces/IOptionsSvc.h"
#endif

//
// method implementations
//

namespace asg
{
  namespace detail
  {
    AnaToolShare ::
    AnaToolShare (const ToolHandle<interfaceType_t>& val_th,
		  std::shared_ptr<void> val_cleanup,
                  std::vector<std::function<StatusCode ()>>&& extraInit)
      : m_th (val_th), m_extraInit (std::move (extraInit)), m_cleanup (std::move (val_cleanup))
    {
      assert (!m_th.empty());
    }



    ToolHandle<interfaceType_t>& AnaToolShare ::
    th ()
    {
      assert (!m_th.empty());
      return m_th;
    }



    AnaToolShareList& AnaToolShareList ::
    singleton () noexcept
    {
      static AnaToolShareList result ATLAS_THREAD_SAFE;
      return result;
    }



    std::shared_ptr<AnaToolShare> AnaToolShareList ::
    getShare (const std::string& name) const
    {
      lock_t lock (m_mutex);
      auto iter = m_shared.find (name);
      if (iter == m_shared.end())
	return std::shared_ptr<AnaToolShare> ();
      return iter->second.lock();
    }



    std::shared_ptr<AnaToolShare> AnaToolShareList ::
    setShare (const std::string& name,
	      std::unique_ptr<AnaToolShare> val_share)
    {
      lock_t lock (m_mutex);
      std::shared_ptr<AnaToolShare> result = getShare (name);
      if (result != nullptr)
	return result;
      result.reset (val_share.release ());
      m_shared[name] = result;
      return result;
    }



    StatusCode AnaToolShareList ::
    makeShare (const std::string& name,
	       const AsgToolConfig& config,
               std::vector<std::function<StatusCode ()>>&& extraInit,
	       std::shared_ptr<AnaToolShare>& result)
    {
      lock_t lock (m_mutex);
      using namespace msgToolHandle;

      auto& share = m_shared[name];
      auto res_result = share.lock();
      if (res_result != nullptr)
      {
	result = res_result;
	return StatusCode::SUCCESS;
      }
      ToolHandle<interfaceType_t> th;
      std::shared_ptr<void> cleanup;
      for (auto& init : extraInit)
        ANA_CHECK (init());
      ANA_CHECK (config.makeTool (th, cleanup, true));
      ANA_MSG_DEBUG ("made shared tool with " << extraInit.size() << " inits for TH: " << th);
      res_result.reset (new AnaToolShare (th, cleanup, std::move (extraInit)));
#ifndef XAOD_STANDALONE
      if (!th.empty())
      {
	th->release ();
      }
#endif
      assert (share.lock() == nullptr);
      share = res_result;
      result = res_result;
      return StatusCode::SUCCESS;
    }
  }
}

//
// legacy code
//

#ifdef XAOD_STANDALONE

namespace asg
{
  namespace detail
  {
    StatusCode makeToolRootCore (const std::string& type,
				 const std::string& name,
				 AsgTool*& tool)
    {
      using namespace msgToolHandle;

      std::regex typeExpr ("[A-Za-z_][A-Za-z0-9_]*(::[A-Za-z_][A-Za-z0-9_]*)*");
      if (!std::regex_match (type, typeExpr))
      {
        ANA_MSG_ERROR ("type \"" << type << "\" does not match format expression");
        return StatusCode::FAILURE;
      }
      std::regex nameExpr ("[A-Za-z_][A-Za-z0-9_]*((\\.|::)[A-Za-z_][A-Za-z0-9_]*)*");
      if (!std::regex_match (name, nameExpr))
      {
        ANA_MSG_ERROR ("name \"" << name << "\" does not match format expression");
        return StatusCode::FAILURE;
      }

      // Load the ROOT dictionary.  Apparently that is needed in some
      // cases to make the line below work, i.e. some dictionaries
      // work, other dictionaries fail.
      for (const char *typeName : {"asg::AsgTool", type.c_str()})
      {
        TClass* toolClass = TClass::GetClass (typeName);
        if(!toolClass){
          ANA_MSG_ERROR("Unable to load class dictionary for type " << type);
          return StatusCode::FAILURE;
        }
      }
 
      ANA_MSG_DEBUG ("Creating tool of type " << type);
      tool = (AsgTool*) (gInterpreter->Calc(("dynamic_cast<asg::AsgTool*>(new " + type + " (\"" + name + "\"))").c_str()));
      if (tool == nullptr)
      {
        ANA_MSG_ERROR ("failed to create tool of type " << type);
        ANA_MSG_ERROR ("make sure you created a dictionary for your tool");
        return StatusCode::FAILURE;
      }

      ANA_MSG_DEBUG ("Created tool of type " << type);

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
      ServiceHandle<Gaudi::Interfaces::IOptionsSvc> svc("JobOptionsSvc","AnaToolHandle");
      if( svc.retrieve().isFailure() ) return StatusCode::FAILURE;
      auto props = svc->items(std::regex("^" + toolName + "\\."));
      StatusCode out = StatusCode::FAILURE;
      for (auto& prop : svc->items())
      {
        if (std::get<0>(prop).substr (0, toolName.size()) == toolName &&
            std::get<0>(prop)[toolName.size()] == '.')
          out = StatusCode::SUCCESS;
      }
      svc.release().ignore();
      //delete props;
      return out;
   }

    StatusCode addPropertyToCatalogue( const std::string& toolName, const std::string& propertyName, const std::string& propertyValue ) {
//std::cout << "Adding " << toolName << " ." << propertyName << " = " << propertyValue << std::endl;
      ServiceHandle<Gaudi::Interfaces::IOptionsSvc> joSvc("JobOptionsSvc","AnaToolHandle");
      //check if propertyName contains '.' . If it does, then assume settng a property of a private tool, so adjust toolname
      joSvc->set( toolName + "." + propertyName, propertyValue );
      if(joSvc.release().isFailure()) return StatusCode::FAILURE;
      return StatusCode::SUCCESS;
   }



   StatusCode removePropertyFromCatalogue( const std::string& toolName, const std::string& propertyName ) {
      ServiceHandle<Gaudi::Interfaces::IOptionsSvc> joSvc("JobOptionsSvc","AnaToolHandle");
      //check if propertyName contains '.' . If it does, then assume settng a property of a private tool, so adjust toolname
      std::string theToolName = toolName; std::string thePropertyName=propertyName;
      std::string::size_type dotLocation = thePropertyName.find_last_of('.');
      if(dotLocation != std::string::npos) {
         theToolName = toolName + "." + thePropertyName.substr(0,dotLocation);
         thePropertyName = thePropertyName.substr(dotLocation+1,thePropertyName.length()-dotLocation);
      }
      joSvc->pop( theToolName + "." + thePropertyName );
      if(joSvc.release().isFailure()) return StatusCode::FAILURE;
      return StatusCode::SUCCESS;
   }
    StatusCode toolExists( const std::string& fullName, interfaceType_t*& tool ) {
      ServiceHandle<IToolSvc> toolSvc("ToolSvc","AnaToolHandle");
      toolSvc.retrieve().ignore();
      auto tools = toolSvc->getTools();
      StatusCode out(StatusCode::FAILURE);
      for(auto atool : tools) {
         if(atool && atool->name() == fullName) {out=StatusCode::SUCCESS;tool=atool;break;}
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

      ServiceHandle<Gaudi::Interfaces::IOptionsSvc> joSvc("JobOptionsSvc","AnaToolHandle");
      auto fromProps = joSvc->items(std::regex ("^" + fromTool));
      for(auto& prop : fromProps) {
        std::get<0>(prop).replace (0, fromTool.size(), toTool);
        joSvc->set( std::get<0>(prop) , std::get<1>(prop) );
      }
      if(joSvc.release().isFailure()) return StatusCode::FAILURE;
      return StatusCode::SUCCESS;
   }


   StatusCode readToolConfig (AsgToolConfig& config, const std::string& toolName) {
     using namespace msgToolHandle;
     //this method copies any properties assigned to tool with 'toolName' to 'config'
     ServiceHandle<Gaudi::Interfaces::IOptionsSvc> joSvc("JobOptionsSvc","AnaToolHandle");
     decltype(auto) fromProps = joSvc->items(std::regex ("^" + toolName));
     for(const auto& prop : fromProps) {
       if (std::get<0>(prop) == toolName)
       {
         config.setTypeAndName (std::get<1>(prop));
       } else
       {
         config.setPropertyFromString (std::get<0>(prop).substr (toolName.size()+1) , std::get<1>(prop));
       }
     }
     ANA_CHECK (joSvc.release());
     return StatusCode::SUCCESS;
   }
  }
}
#endif
