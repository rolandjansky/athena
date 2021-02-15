/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgTools/AsgComponentConfig.h>

#include <AsgTools/AsgToolConfig.h>
#include <regex>

#ifdef XAOD_STANDALONE

#include <AsgTools/AsgComponent.h>
#include <AsgTools/AsgTool.h>
#include <AsgTools/AnaToolHandle.h>
#include <TInterpreter.h>
#include <boost/format.hpp>

#else

#include "Gaudi/Interfaces/IOptionsSvc.h"
#include <GaudiKernel/ServiceHandle.h>

#endif

//
// method implementations
//

namespace asg
{
  AsgComponentConfig ::
  AsgComponentConfig (const std::string& val_typeAndName)
  {
    setTypeAndName (val_typeAndName);
  }



  bool AsgComponentConfig ::
  empty () const noexcept
  {
    return
      m_type.empty() && m_name.empty() &&
      m_privateTools.empty() && m_propertyValues.empty();
  }



  const std::string& AsgComponentConfig ::
  type () const noexcept
  {
    return m_type;
  }



  void AsgComponentConfig ::
  setType (const std::string& val_type)
  {
    m_type = val_type;
  }



  const std::string& AsgComponentConfig ::
  name () const noexcept
  {
    return m_name;
  }



  void AsgComponentConfig ::
  setName (const std::string& val_name)
  {
    m_name = val_name;
  }



  std::string AsgComponentConfig ::
  typeAndName () const
  {
    if (m_name == m_type)
      return m_name;
    return m_type + "/" + m_name;
  }



  void AsgComponentConfig ::
  setTypeAndName (const std::string& val_typeAndName)
  {
    const auto split = val_typeAndName.find ('/');
    if (split == std::string::npos)
    {
      setType (val_typeAndName);
      setName (val_typeAndName);
    } else
    {
      setType (val_typeAndName.substr (0,split));
      setName (val_typeAndName.substr (split+1));
    }
  }



  void AsgComponentConfig ::
  setPropertyFromString (const std::string& name,
                         const std::string& value)
  {
    m_propertyValues[name] = value;
  }



  StatusCode AsgComponentConfig ::
  createPrivateTool (const std::string& name,
                     const std::string& toolType)
  {
    m_privateTools[name] = toolType;
    return StatusCode::SUCCESS;
  }



  StatusCode AsgComponentConfig ::
  checkTypeName (bool nestedNames) const
  {
    using namespace msgComponentConfig;

    std::regex typeExpr ("[A-Za-z_][A-Za-z0-9_]*(::[A-Za-z_][A-Za-z0-9_]*)*");
    if (!std::regex_match (m_type, typeExpr))
    {
      ANA_MSG_ERROR ("type \"" << m_type << "\" does not match format expression");
      return StatusCode::FAILURE;
    }
    std::regex nameExpr;
    if (nestedNames)
      nameExpr = std::regex ("[A-Za-z_][A-Za-z0-9_]*(\\.[A-Za-z_][A-Za-z0-9_]*)*");
    else
      nameExpr = std::regex ("[A-Za-z_][A-Za-z0-9_]*");
    if (!std::regex_match (m_name, nameExpr))
    {
      ANA_MSG_ERROR ("name \"" << m_name << "\" does not match format expression");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  StatusCode AsgComponentConfig ::
  addPrivateTool (const std::string& name,
                  const AsgToolConfig& toolConfig)
  {
    using namespace msgComponentConfig;

    ANA_CHECK (createPrivateTool (name, toolConfig.type()));
    for (const auto& subtool : toolConfig.m_privateTools)
      ANA_CHECK (createPrivateTool (name + "." + subtool.first, subtool.second));
    for (const auto& property : toolConfig.m_propertyValues)
      setPropertyFromString (name + "." + property.first, property.second);
    return StatusCode::SUCCESS;
  }



#ifdef XAOD_STANDALONE
  namespace
  {
    /// \brief count the number of separators in a tool name
    /// \par Guarantee
    ///   no-fail
    std::size_t countSeparators (const std::string& name)
    {
      std::size_t result {!name.empty()};
      for (char ch : name)
      {
        if (ch == '.')
          result += 1;
      }
      return result;
    }



    /// \brief the map of all known components
    struct ComponentMap final
    {
      /// \brief the list of tools we created/know about
      std::map<std::string,AsgComponent*> m_components;

      /// \brief the tool cleanup list
      std::list<std::shared_ptr<void> > m_cleanup;


      /// \brief set a property on the component or a sub-tool
      /// \par Guarantee
      ///   basic
      /// \par Failures
      ///   could not set property\n
      ///   property not found\n
      ///   sub-tool not found

      StatusCode setProperty (const std::string& name,
                                const std::string& value)
      {
        using namespace msgComponentConfig;

        std::string componentName;
        std::string propertyName;

        const auto split = name.rfind (".");
        if (split == std::string::npos)
        {
          propertyName = name;
        } else
        {
          componentName = name.substr (0, split);
          propertyName = name.substr (split+1);
        }

        const auto component = m_components.find (componentName);
        if (component == m_components.end())
        {
          ANA_MSG_ERROR ("trying to set property \"" << propertyName << "\" on component \"" << componentName << "\" which has not been configured");
          return StatusCode::FAILURE;
        }

        return component->second->setProperty (propertyName, value);
      }



      /// \brief initialize all the tools
      ///
      /// need to initialize the tools inside out, i.e. sub-sub-tools
      /// before sub-tools, etc. and then set them as properties on
      /// their parents
      /// \par Guarantee
      ///   basic
      /// \par Failures
      ///   out of memory II\n
      ///   tool initialization failures
      StatusCode initializeTools ()
      {
        using namespace msgComponentConfig;

        std::vector<std::pair<std::string,AsgTool*> > sortedTools;
        for (auto& component : m_components)
        {
          // we skip the top-level component which will be initialized
          // by the component specific wrapper
          if (!component.first.empty())
          {
            AsgTool *tool = dynamic_cast<AsgTool*>(component.second);

            if (tool == nullptr)
            {
              ANA_MSG_ERROR ("configured non-tool component as subtool: " << component.first);
              return StatusCode::FAILURE;
            }
            sortedTools.emplace_back (component.first, tool);
          }
        }
        std::sort (sortedTools.begin(), sortedTools.end(), [] (auto& a, auto& b) {
            const std::size_t levela = countSeparators (a.first);
            const std::size_t levelb = countSeparators (b.first);
            if (levela > levelb) return true;
            if (levela < levelb) return false;
            return a.first < b.first;});
        for (const auto& tool : sortedTools)
        {
          ANA_CHECK (tool.second->initialize());

          // using that a ToolHandle initialized with a tool name will
          // retrieve that tool, not sure if that is the best strategy
          ANA_CHECK (setProperty (tool.first, tool.second->name()));
        }
        return StatusCode::SUCCESS;
      }
    };



    StatusCode createComponent (std::unique_ptr<AsgComponent>& component,
                                const std::string& type,
                                const std::string& name,
                                const std::string& newCommand)
    {
      using namespace msgComponentConfig;

      // Load the ROOT dictionary, this is needed to be able to
      // instantiate the component below, i.e. the code below won't load
      // dictionaries not already loaded
      TClass* componentClass = TClass::GetClass (type.c_str());
      if (!componentClass)
      {
        ATH_MSG_ERROR ("Unable to load class dictionary for type " << type);
        return StatusCode::FAILURE;
      }

      AsgComponent *comp = reinterpret_cast<AsgComponent*>
        (gInterpreter->Calc(("dynamic_cast<asg::AsgComponent*>(" + (boost::format (newCommand) % type % name).str() + ")").c_str()));
      if (comp == nullptr)
      {
        ANA_MSG_ERROR ("failed to create component of type " << type);
        ANA_MSG_ERROR ("make sure you created a dictionary for your component");
        return StatusCode::FAILURE;
      }
      component.reset (comp);

      return StatusCode::SUCCESS;
    }



    StatusCode createTool (const std::string& type,
                           const std::string& name,
                           std::shared_ptr<asg::AsgTool>& tool)
    {
      using namespace msgComponentConfig;

      // ideally we move the makeToolRootCore out of the detail
      // namespace, but for now I just want to make this work.
      asg::AsgTool *rawTool = nullptr;
      if (!asg::detail::makeToolRootCore (type, name, rawTool).isSuccess())
        return StatusCode::FAILURE;
      tool.reset (rawTool);
      return StatusCode::SUCCESS;
    }
  }



  template<> StatusCode AsgComponentConfig ::
  makeComponentExpert<AsgComponent> (std::unique_ptr<AsgComponent>& component,
                                     const std::string& newCommand,
                                     bool nestedNames, std::string prefix) const
  {
    using namespace msgComponentConfig;

    ANA_CHECK (checkTypeName (nestedNames));

    std::string name = prefix + m_name;

    ComponentMap componentMap;

    if (!createComponent (component, m_type, name, newCommand).isSuccess())
      return StatusCode::FAILURE;
    componentMap.m_components.insert (std::make_pair ("", component.get()));

    for (auto& toolInfo : m_privateTools)
    {
      std::shared_ptr<asg::AsgTool> tool;
      if (!createTool (toolInfo.second, name + "." + toolInfo.first, tool).isSuccess())
        return StatusCode::FAILURE;
      componentMap.m_cleanup.push_front (tool);
      componentMap.m_components.insert (std::make_pair (toolInfo.first, tool.get()));
    }

    for (auto& property : m_propertyValues)
    {
      ANA_CHECK (componentMap.setProperty (property.first, property.second));
    }

    if (!componentMap.initializeTools ().isSuccess())
      return StatusCode::FAILURE;

    for (auto& cleanup : componentMap.m_cleanup)
      component->addCleanup (cleanup);

    ANA_MSG_DEBUG ("Created component of type " << m_type);
    return StatusCode::SUCCESS;
  }



#else



  StatusCode AsgComponentConfig ::
  configureComponentExpert (const std::string& prefix,
                            bool nestedNames) const
  {
    using namespace msgComponentConfig;

    ANA_CHECK (checkTypeName (nestedNames));

    ServiceHandle<Gaudi::Interfaces::IOptionsSvc> joSvc("JobOptionsSvc","AsgComponentConfig");
    ANA_CHECK (joSvc.retrieve());

    for (const auto& tool : m_privateTools)
    {
      std::string toolPath = prefix + m_name + "." + tool.first;
      const auto split = toolPath.rfind ('.');
      std::string toolName = toolPath.substr (split+1);
      joSvc->set (toolPath, tool.second + "/" + toolName);
    }

    for (const auto& property : m_propertyValues)
    {
      std::string propertyPath = prefix + m_name + "." + property.first;
      joSvc->set (propertyPath, property.second);
    }

    return StatusCode::SUCCESS;
  }

#endif
}
