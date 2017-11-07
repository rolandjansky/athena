//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//
//        
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

#ifdef XAOD_STANDALONE
#include <TInterpreter.h>
#include <TROOT.h>
#include <regex>
#else
#include <GaudiKernel/AlgTool.h>
#include <AsgTools/MessageCheck.h>
#include <GaudiKernel/IToolSvc.h>
#include <GaudiKernel/IJobOptionsSvc.h>
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
		  AnaToolCleanup val_cleanup)
      : m_th (val_th), m_cleanup (std::move (val_cleanup))
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
      static AnaToolShareList result;
      return result;
    }



    std::shared_ptr<AnaToolShare> AnaToolShareList ::
    getShare (const std::string& name) const
    {
      auto iter = m_shared.find (name);
      if (iter == m_shared.end())
	return std::shared_ptr<AnaToolShare> ();
      return iter->second.lock();
    }



    std::shared_ptr<AnaToolShare> AnaToolShareList ::
    setShare (const std::string& name,
	      std::unique_ptr<AnaToolShare> val_share)
    {
      std::shared_ptr<AnaToolShare> result = getShare (name);
      if (result != nullptr)
	return result;
      result.reset (val_share.release ());
      m_shared[name] = result;
      return result;
    }



    StatusCode AnaToolShareList ::
    makeShare (const std::string& name,
	       const AnaToolConfig& config,
	       std::shared_ptr<AnaToolShare>& result)
    {
      using namespace msgToolHandle;

      auto& share = m_shared[name];
      auto res_result = share.lock();
      if (res_result != nullptr)
      {
	result = res_result;
	return StatusCode::SUCCESS;
      }
      ToolHandle<interfaceType_t> th;
      AnaToolCleanup cleanup;
      ANA_CHECK (config.makeTool (name, nullptr, th, cleanup));
      res_result.reset (new AnaToolShare (th, cleanup));
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
      std::regex nameExpr ("[A-Za-z_][A-Za-z0-9_]*(.[A-Za-z_][A-Za-z0-9_]*)*");
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
      ServiceHandle<IJobOptionsSvc> svc("JobOptionsSvc","AnaToolHandle");
      if( svc.retrieve().isFailure() ) return StatusCode::FAILURE;
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

namespace asg
{
  namespace detail
  {
    void AnaToolConfig ::
    swap (AnaToolConfig& that) noexcept
    {
      m_type.swap (that.m_type);
      m_factory.swap (that.m_factory);
      m_properties.swap (that.m_properties);
    }



    bool AnaToolConfig ::
    empty () const noexcept
    {
      if (!m_properties.empty())
	return false;
      if (!m_type.empty())
	return false;
#ifdef XAOD_STANDALONE
      if (m_factory)
	return false;
#endif
      return true;
    }



    const std::string& AnaToolConfig ::
    type () const noexcept
    {
      return m_type;
    }



    void AnaToolConfig ::
    setType (std::string type) noexcept
    {
      m_type = std::move (type);
    }



    void AnaToolConfig ::
    addProperty (const std::string& name,
		 const std::shared_ptr<AnaToolProperty>& property)
    {
      m_properties[name] = property;
    }



#ifdef XAOD_STANDALONE
  StatusCode AnaToolConfig ::
  allocateTool (AsgTool*& toolPtr, const std::string& toolName) const
  {
    using namespace msgToolHandle;

    if (m_factory)
    {
      ANA_CHECK (m_factory (toolPtr, toolName));
      return StatusCode::SUCCESS;
    }

    try
    {
      ANA_CHECK (detail::makeToolRootCore (type(), toolName, toolPtr));
    } catch (std::exception& e)
    {
      ANA_MSG_ERROR ("encountered exception during tool creation(" <<
		     type() << "," << toolName << "): " << e.what());
      return StatusCode::FAILURE;
    }
    if (toolPtr == nullptr)
    {
      ANA_MSG_ERROR ("failed to allocate tool, this is typically caused either by the tool inheriting from AsgTool virtually or because its first base class is not AsgTool): " + type());
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }
#endif



#ifndef XAOD_STANDALONE
    StatusCode AnaToolConfig ::
    applyPropertiesAthena (const std::string& toolName,
			   AnaToolCleanup& cleanup) const
    {
      using namespace msgToolHandle;

      for (auto& property : m_properties)
      {
	ANA_CHECK (property.second->applyPropertyAthena
		   (toolName, property.first, cleanup));
      }
      return StatusCode::SUCCESS;
    }
#endif



    /*
    namespace
    {
      /// convert the cleanup list into a single cleanup pointer
      std::shared_ptr<void>
      compressCleanup (std::list<std::shared_ptr<void> >& baseCleanup)
      {
	if (baseCleanup.empty())
	  return std::shared_ptr<void> ();
	if (baseCleanup.size() == 1)
	  return baseCleanup.front();
	return std::shared_ptr<std::vector<std::shared_ptr<void> > >
	  (new std::vector<std::shared_ptr<void> > (baseCleanup.begin(),
						    baseCleanup.end()));
      }
    }
    */



#ifdef XAOD_STANDALONE
    StatusCode AnaToolConfig ::
    makeToolRootCore (const std::string& toolName, IAsgTool*& toolPtr,
		      AnaToolCleanup& cleanup) const
    {
      using namespace msgToolHandle;

      asg::AsgTool *rawToolPtr = nullptr;
      ANA_CHECK (allocateTool (rawToolPtr, toolName));
      std::shared_ptr<AsgTool> sharedPtr (rawToolPtr);
      AnaToolCleanup mycleanup;

      for (auto& property : m_properties)
      {
	ANA_CHECK (property.second->applyPropertyRootCore
		     (*rawToolPtr, property.first, mycleanup));
      }
      ANA_CHECK (rawToolPtr->initialize());

      mycleanup.addCleanup (sharedPtr, false);
      cleanup.swap (mycleanup);
      toolPtr = sharedPtr.get();
      return StatusCode::SUCCESS;
    }
#endif


#ifndef XAOD_STANDALONE
    /// \brief manage the reference count on a tool

    struct ToolRefManager
    {
      ToolRefManager (const ToolHandle<IAlgTool>& val_tool, bool addRef)
	: m_tool (val_tool), m_releases (m_tool->refCount())
      {
	if (addRef)
	{
	  m_tool->addRef();
	  ++ m_releases;
	}
      }

      ~ToolRefManager ()
      {
        using namespace msgToolHandle;
        assert (m_releases <= m_tool->refCount());
        if (m_releases > 0)
        {
          // for (unsigned iter = 1, end = m_tool->refCount(); iter != end; ++ iter)
          for (unsigned iter = 1; iter != m_releases; ++ iter)
            m_tool->release();
          ANA_CHECK_THROW (m_tool.release());
        }
      }

      ToolHandle<IAlgTool> m_tool;
      unsigned m_releases = 0;
    };



    StatusCode AnaToolPropertyValueAthena ::
    applyPropertyAthena (const std::string& toolName,
			 const std::string& name,
			 AnaToolCleanup& /*cleanup*/)
      const
    {
      using namespace msgToolHandle;

      // this is so we clear out properties from catalogue when tool
      // is destroyed. DISABLED: we no longer clear out tools in Athena
      // std::shared_ptr<PropertyValueManager> manager
      //   (new PropertyValueManager (toolName, name));
      // cleanup.addCleanup (manager);

      // for athena we hand our property to the joboptions svc
      ANA_CHECK (detail::addPropertyToCatalogue (toolName, name, m_value));
      // manager->m_cleanup = true;
      return StatusCode::SUCCESS;
    }
#endif



    StatusCode AnaToolConfig ::
    makeBaseTool (const std::string& name,
                  parentType_t *parent,
		  ToolHandle<interfaceType_t>& th,
		  AnaToolCleanup& cleanup) const
    {
      using namespace msgToolHandle;

      std::string toolName;
      if (parent)
        toolName = parent->name() + "." + name;
      else
        toolName = "ToolSvc." + name;

#ifdef XAOD_STANDALONE
      interfaceType_t *baseToolPtr = nullptr;
      AnaToolCleanup baseCleanup;
      ANA_CHECK (makeToolRootCore (toolName, baseToolPtr, baseCleanup));
      th = ToolHandle<interfaceType_t> (baseToolPtr);
      cleanup.swap (baseCleanup);
#else
      AnaToolCleanup baseCleanup;
      ANA_CHECK (applyPropertiesAthena (toolName, baseCleanup));
      ToolHandle<interfaceType_t> myth (m_type + "/" + name, parent);
      ANA_CHECK (myth.retrieve());
      // disabling this as it causes crashes in offline.  if you
      // re-enable this, also reenable the cleanup tools in
      // applyPropertiesAthena
      // std::shared_ptr<detail::ToolRefManager> manager
      //   (new detail::ToolRefManager (myth, true));
      // baseCleanup.addCleanup (manager, false);
      th = myth;
      myth->release ();
      cleanup.swap (baseCleanup);
#endif
      return StatusCode::SUCCESS;
    }



    void AnaToolCleanup ::
    swap (AnaToolCleanup& that)
    {
      m_cleanup.swap (that.m_cleanup);
    }



    void AnaToolCleanup ::
    addCleanup (AnaToolCleanup val_cleanup, bool post)
    {
      if (post)
      {
	for (auto& cleanup : val_cleanup.m_cleanup)
	  m_cleanup.push_back (cleanup);
      } else
      {
	val_cleanup.addCleanup (*this, true);
	m_cleanup.swap (val_cleanup.m_cleanup);
      }
    }



    void AnaToolCleanup ::
    addCleanup (const std::shared_ptr<void>& val_cleanup,
		bool post)
    {
      if (val_cleanup)
      {
	if (post)
	  m_cleanup.push_back (val_cleanup);
	else
	  m_cleanup.push_front (val_cleanup);
      }
    }



    StatusCode AnaToolConfig ::
    setProperty (const std::string& val_name, const char *val_value)
    {
      return setProperty (val_name, std::string (val_value));
    }



#ifndef XAOD_STANDALONE
    AnaToolPropertyCopyTool ::
    AnaToolPropertyCopyTool (const std::string& val_type,
                             const std::string& val_name)
      : m_type (val_type), m_name (val_name)
    {
    }

    StatusCode AnaToolPropertyCopyTool ::
    applyPropertyAthena (const std::string& toolName,
                         const std::string& name,
                         AnaToolCleanup& /*cleanup*/)
      const
    {
      using namespace msgToolHandle;

      std::string myname = toolName + "." + name;

      ANA_CHECK (addPropertyToCatalogue (toolName, name, m_type + "/" + name));
      ANA_CHECK (copyPropertiesInCatalogue (m_name, myname));
      return StatusCode::SUCCESS;
    }
#endif
  }
}
