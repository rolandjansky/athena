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

#include <GaudiKernel/IJobOptionsSvc.h>
#include <GaudiKernel/ServiceHandle.h>

#endif

//
// method implementations
//

namespace asg
{
  namespace
  {
    /// make the name of a private tool in an array
    ///
    /// I'm not sure if there is a specific naming convention for
    /// tools in tool arrays, and I'm not sure if it matters.  So this
    /// allows to change the naming scheme of private tools in arrays
    /// at will, if needed.
    std::string makeArrayName (const std::string& name, std::size_t index)
    {
      return name + "@" + std::to_string (index);
    }
  }


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
    auto split = name.find ('.');
    if (split == std::string::npos)
    {
      m_propertyValues[name] = value;
    } else
    {
      auto subtool = accessSubtool (name, split);
      subtool.config->setPropertyFromString (subtool.name, value);
    }
  }



  StatusCode AsgComponentConfig ::
  createPrivateTool (const std::string& name,
                     const std::string& toolType)
  {
    return addPrivateTool (name, AsgToolConfig (toolType + "/" + name));
  }



  std::string AsgComponentConfig ::
  createPrivateToolInArray (const std::string& name,
                            const std::string& toolType)
  {
    return addPrivateToolInArray (name, AsgToolConfig (toolType + "/" + name));
  }



  StatusCode AsgComponentConfig ::
  addPrivateTool (const std::string& name,
                  AsgToolConfig toolConfig)
  {
    using namespace msgComponentConfig;

    auto split = name.find ('.');
    if (split == std::string::npos)
    {
      toolConfig.setName (name);
      m_privateTools[name] = std::make_tuple (std::move (toolConfig), "");
      return StatusCode::SUCCESS;
    } else
    {
      auto subtool = accessSubtool (name, split);
      return subtool.config->addPrivateTool (subtool.name, std::move (toolConfig));
    }
  }



  std::string AsgComponentConfig ::
  addPrivateToolInArray (const std::string& name,
                         AsgToolConfig toolConfig)
  {
    using namespace msgComponentConfig;

    auto split = name.find ('.');
    if (split == std::string::npos)
    {
      auto& arrayData = m_toolArrays[name];
      auto myname = makeArrayName (name, arrayData.size());
      toolConfig.setName (myname);
      m_privateTools.emplace (myname, std::make_tuple (std::move (toolConfig), name));
      arrayData.push_back (myname);
      return myname;
    } else
    {
      auto subtool = accessSubtool (name, split);
      return subtool.prefix + subtool.config
        ->addPrivateToolInArray (subtool.name, std::move (toolConfig));
    }
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
      nameExpr = std::regex ("[A-Za-z_][A-Za-z0-9_]*(\\.[A-Za-z_][A-Za-z0-9_]*)*(@[0-9]+)?");
    else
      nameExpr = std::regex ("[A-Za-z_][A-Za-z0-9_]*(@[0-9]+)?");
    if (!std::regex_match (m_name, nameExpr))
    {
      ANA_MSG_ERROR ("name \"" << m_name << "\" does not match format expression");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  AsgComponentConfig::AccessSubtoolData AsgComponentConfig ::
  accessSubtool (const std::string& name, std::size_t split)
  {
    AccessSubtoolData result;
    auto subtool = m_privateTools.find (name.substr (0, split));
    if (subtool == m_privateTools.end())
      throw std::runtime_error ("trying to access unknown private tool: " + name.substr (0, split));
    result.config = &std::get<0>(subtool->second);
    result.prefix = name.substr (0, split + 1);
    result.name = name.substr (split + 1);
    return result;
  }



#ifdef XAOD_STANDALONE
  namespace
  {
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
  }



  template<> StatusCode AsgComponentConfig ::
  makeComponentExpert<AsgComponent> (std::unique_ptr<AsgComponent>& component,
                                     const std::string& newCommand,
                                     bool nestedNames, std::string prefix) const
  {
    using namespace msgComponentConfig;

    ANA_CHECK (checkTypeName (nestedNames));

    std::string name = prefix + m_name;

    if (!createComponent (component, m_type, name, newCommand).isSuccess())
      return StatusCode::FAILURE;

    for (auto& toolInfo : m_privateTools)
    {
      ToolHandle<AsgTool> th (toolInfo.first, component.get());
      std::shared_ptr<void> mycleanup;
      if (std::get<0>(toolInfo.second).makeTool (th, mycleanup).isFailure())
      {
        ANA_MSG_ERROR ("failed to create subtool \"" << toolInfo.first << "\" on component \"" << component->name() << "\"");
        return StatusCode::FAILURE;
      }
      component->addCleanup (mycleanup);
      if (std::get<1>(toolInfo.second).empty())
      {
        if (component->setProperty (toolInfo.first, th->name()).isFailure())
        {
          ANA_MSG_ERROR ("failed to set ToolHandle property \"" << toolInfo.first << "\" on component \"" << component->name() << "\"");
          return StatusCode::FAILURE;
        }
      }
    }

    for (const auto& toolArray : m_toolArrays)
    {
      std::vector<std::string> valueArray;
      for (const auto& tool : toolArray.second)
        valueArray.emplace_back (component->name() + "." + tool);
      std::string valueString;
      ANA_CHECK (asg::detail::GetCastStringHelper<std::vector<std::string>>::get (valueArray, valueString));
      ANA_CHECK (component->setProperty (toolArray.first, valueString));
    }

    for (auto& property : m_propertyValues)
    {
      ANA_CHECK (component->setProperty (property.first, property.second));
    }

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

    ServiceHandle<IJobOptionsSvc> joSvc("JobOptionsSvc","AsgComponentConfig");
    ANA_CHECK (joSvc.retrieve());

    for (const auto& tool : m_privateTools)
    {
      ANA_CHECK (std::get<0>(tool.second).configureComponentExpert (prefix + m_name + ".", true));
      if (std::get<1>(tool.second).empty())
      {
        std::string toolPath = prefix + m_name + "." + tool.first;
        const auto split = toolPath.rfind ('.');
        std::string toolName = toolPath.substr (split+1);
        std::string componentName = toolPath.substr (0, split);
        StringProperty athenaProperty (toolName, std::get<0>(tool.second).typeAndName());
        ANA_CHECK (joSvc->addPropertyToCatalogue (componentName, std::move (athenaProperty)));
      }
    }

    for (const auto& toolArray : m_toolArrays)
    {
      std::vector<std::string> valueArray;
      for (const auto& tool : toolArray.second)
      {
        auto toolConfig = m_privateTools.find (tool);
        valueArray.push_back (std::get<0>(toolConfig->second).typeAndName());
      }
      std::string valueString = Gaudi::Utils::toString (valueArray);
      std::string propertyPath = prefix + m_name + "." + toolArray.first;
      const auto split = propertyPath.rfind ('.');
      std::string propertyName = propertyPath.substr (split+1);
      std::string componentName = propertyPath.substr (0, split);
      StringProperty athenaProperty (propertyName, valueString);
      ANA_CHECK (joSvc->addPropertyToCatalogue (componentName, std::move (athenaProperty)));
    }

    for (const auto& property : m_propertyValues)
    {
      std::string propertyPath = prefix + m_name + "." + property.first;
      const auto split = propertyPath.rfind ('.');
      std::string propertyName = propertyPath.substr (split+1);
      std::string componentName = propertyPath.substr (0, split);
      StringProperty athenaProperty (propertyName, property.second);
      ANA_CHECK (joSvc->addPropertyToCatalogue (componentName, std::move (athenaProperty)));
    }

    return StatusCode::SUCCESS;
  }

#endif
}
