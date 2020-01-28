/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AnaAlgorithm/AnaAlgorithmConfig.h>

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AsgTools/AsgTool.h>
#include <AsgTools/AnaToolHandle.h>
#include <RootCoreUtils/Assert.h>
#include <TInterpreter.h>
#include <regex>

//
// method implementations
//

namespace EL
{
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



    /// \brief the information about a single component (algorithm or
    /// tool)
    struct Component final
    {
      AnaAlgorithm *m_algorithm {};
      asg::AsgTool *m_tool {};

      Component () = default;
      explicit Component (AnaAlgorithm *val_algorithm)
        : m_algorithm (val_algorithm) {}
      explicit Component (asg::AsgTool *val_tool)
        : m_tool (val_tool) {}

      const std::string& name () const
      {
        if (m_tool)
          return m_tool->name();
        assert (m_algorithm != nullptr);
        return m_algorithm->name();
      }

      StatusCode initialize () const
      {
        if (m_tool)
          return m_tool->initialize();
        assert (m_algorithm != nullptr);
        return m_algorithm->sysInitialize();
      }



      StatusCode setProperty (const std::string& propertyName,
                              const std::string& value) const
      {
        using namespace msgAlgorithmConfig;

        if (m_tool)
        {
          if (!m_tool->setProperty (propertyName, value).isSuccess())
          {
            ANA_MSG_ERROR ("failed to set property \"" << propertyName << "\" with value \"" << value << "\" on tool \"" << name() << "\"");
            return StatusCode::FAILURE;
          }
          return StatusCode::SUCCESS;
        }
        assert (m_algorithm != nullptr);
        if (!m_algorithm->setProperty (propertyName, value).isSuccess())
        {
          ANA_MSG_ERROR ("failed to set property \"" << propertyName << "\" with value \"" << value << "\" on algorithm \"" << name() << "\"");
          return StatusCode::FAILURE;
        }
        return StatusCode::SUCCESS;
      }
    };



    /// \brief the map of all known components
    struct ComponentMap final
    {
      /// \brief the list of tools we created/know about
      std::map<std::string,Component> m_components;

      /// \brief the tool cleanup list
      std::list<std::shared_ptr<void> > m_cleanup;


      /// \brief set a property on an algorithm or sub-tool
      /// \par Guarantee
      ///   basic
      /// \par Failures
      ///   could not set property\n
      ///   property not found\n
      ///   sub-tool not found

      StatusCode setProperty (const std::string& name,
                                const std::string& value)
      {
        using namespace msgAlgorithmConfig;

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

        return component->second.setProperty (propertyName, value);
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
        using namespace msgAlgorithmConfig;

        std::vector<std::pair<std::string,Component> > sortedTools;
        for (auto& component : m_components)
        {
          if (component.second.m_tool)
            sortedTools.push_back (component);
        }
        std::sort (sortedTools.begin(), sortedTools.end(), [] (auto& a, auto& b) {
            const std::size_t levela = countSeparators (a.first);
            const std::size_t levelb = countSeparators (b.first);
            if (levela > levelb) return true;
            if (levela < levelb) return false;
            return a.first < b.first;});
        for (const auto& tool : sortedTools)
        {
          ANA_CHECK (tool.second.initialize());

          // Skip the next step if tool.first is empty. Which is the case for
          // public tools for instance.
          if( tool.first.empty() ) continue;

          // using that a ToolHandle initialized with a tool name will
          // retrieve that tool, not sure if that is the best strategy
          ANA_CHECK (setProperty (tool.first, tool.second.name()));
        }
        return StatusCode::SUCCESS;
      }
    };



    StatusCode createAlgorithm (const std::string& type,
                                const std::string& name,
                                std::unique_ptr<AnaAlgorithm>& algorithm)
    {
      using namespace msgAlgorithmConfig;

      // Load the ROOT dictionary, this is needed to be able to
      // instantiate the algorithm below, i.e. the code below won't load
      // dictionaries not already loaded
      TClass* algorithmClass = TClass::GetClass (type.c_str());
      if (!algorithmClass)
      {
        ATH_MSG_ERROR ("Unable to load class dictionary for type " << type);
        return StatusCode::FAILURE;
      }

      EL::AnaAlgorithm *alg = (EL::AnaAlgorithm*)
        (gInterpreter->Calc(("dynamic_cast<EL::AnaAlgorithm*>(new " + type + " (\"" + name + "\", nullptr))").c_str()));
      if (alg == nullptr)
      {
        ANA_MSG_ERROR ("failed to create algorithm of type " << type);
        ANA_MSG_ERROR ("make sure you created a dictionary for your algorithm");
        return StatusCode::FAILURE;
      }
      algorithm.reset (alg);

      return StatusCode::SUCCESS;
    }



    StatusCode createTool (const std::string& type,
                           const std::string& name,
                           std::shared_ptr<asg::AsgTool>& tool)
    {
      using namespace msgAlgorithmConfig;

      // ideally we move the makeToolRootCore out of the detail
      // namespace, but for now I just want to make this work.
      asg::AsgTool *rawTool = nullptr;
      if (!asg::detail::makeToolRootCore (type, name, rawTool).isSuccess())
        return StatusCode::FAILURE;
      tool.reset (rawTool);
      return StatusCode::SUCCESS;
    }
  }


  void AnaAlgorithmConfig :: 
  testInvariant () const
  {
    RCU_INVARIANT (this != nullptr);
  }



  AnaAlgorithmConfig :: 
  AnaAlgorithmConfig ()
  {
    RCU_NEW_INVARIANT (this);
  }



  AnaAlgorithmConfig ::
  AnaAlgorithmConfig (const std::string& val_typeAndName)
    : AnaAlgorithmConfig ()
  {
    setTypeAndName (val_typeAndName);

    // no invariant used
  }



  const std::string& AnaAlgorithmConfig ::
  type () const noexcept
  {
    RCU_READ_INVARIANT (this);
    return m_type;
  }



  void AnaAlgorithmConfig ::
  setType (const std::string& val_type) noexcept
  {
    RCU_CHANGE_INVARIANT (this);
    m_type = val_type;
  }



  const std::string& AnaAlgorithmConfig ::
  name () const noexcept
  {
    RCU_READ_INVARIANT (this);
    return m_name;
  }



  void AnaAlgorithmConfig ::
  setName (const std::string& val_name) noexcept
  {
    RCU_CHANGE_INVARIANT (this);
    m_name = val_name;
  }



  void AnaAlgorithmConfig ::
  setTypeAndName (const std::string& val_typeAndName)
  {
    // no invariant used
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



  bool AnaAlgorithmConfig ::
  useXAODs () const noexcept
  {
    RCU_READ_INVARIANT (this);
    return m_useXAODs;
  }



  void AnaAlgorithmConfig ::
  setUseXAODs (bool val_useXAODs) noexcept
  {
    RCU_CHANGE_INVARIANT (this);
    m_useXAODs = val_useXAODs;
  }



  bool AnaAlgorithmConfig ::
  isPublicTool () const noexcept
  {
    RCU_READ_INVARIANT (this);
    return m_isPublicTool;
  }



  void AnaAlgorithmConfig ::
  setIsPublicTool (bool val_isPublicTool) noexcept
  {
    RCU_CHANGE_INVARIANT (this);
    m_isPublicTool = val_isPublicTool;
  }



  void AnaAlgorithmConfig ::
  setPropertyFromString (const std::string& name,
                         const std::string& value)
  {
    RCU_CHANGE_INVARIANT (this);
    m_propertyValues[name] = value;
  }



  ::StatusCode AnaAlgorithmConfig ::
  createPrivateTool (const std::string& name,
                     const std::string& toolType)
  {
    RCU_CHANGE_INVARIANT (this);
    m_privateTools[name] = toolType;
    return StatusCode::SUCCESS;
  }



  ::StatusCode AnaAlgorithmConfig ::
  makeAlgorithm (std::unique_ptr<AnaAlgorithm>& algorithm) const
  {
    RCU_READ_INVARIANT (this);
    using namespace msgAlgorithmConfig;

    std::regex typeExpr ("[A-Za-z_][A-Za-z0-9_]*(::[A-Za-z_][A-Za-z0-9_]*)*");
    if (!std::regex_match (m_type, typeExpr))
    {
      ANA_MSG_ERROR ("type \"" << m_type << "\" does not match format expression");
      return StatusCode::FAILURE;
    }
    std::regex nameExpr ("[A-Za-z_][A-Za-z0-9_]*");
    if (!std::regex_match (m_name, nameExpr))
    {
      ANA_MSG_ERROR ("name \"" << m_name << "\" does not match format expression");
      return StatusCode::FAILURE;
    }

    ComponentMap componentMap;

    if (m_isPublicTool == false)
    {
      if (!createAlgorithm (m_type, m_name, algorithm).isSuccess())
        return StatusCode::FAILURE;
      componentMap.m_components.insert (std::make_pair ("", Component (algorithm.get())));
    } else
    {
      std::shared_ptr<asg::AsgTool> tool;
      if (!createTool (m_type, m_name, tool).isSuccess())
        return StatusCode::FAILURE;
      componentMap.m_cleanup.push_front (tool);
      componentMap.m_components.insert (std::make_pair ("", Component (tool.get())));
    }

    for (auto& toolInfo : m_privateTools)
    {
      std::shared_ptr<asg::AsgTool> tool;
      if (!createTool (toolInfo.second, m_name + "." + toolInfo.first, tool).isSuccess())
        return StatusCode::FAILURE;
      componentMap.m_cleanup.push_front (tool);
      componentMap.m_components.insert (std::make_pair (toolInfo.first, Component (tool.get())));
    }

    for (auto& property : m_propertyValues)
    {
      ANA_CHECK (componentMap.setProperty (property.first, property.second));
    }

    if (!componentMap.initializeTools ().isSuccess())
      return StatusCode::FAILURE;

    if (m_isPublicTool == true)
    {
      assert (algorithm == nullptr);
      if (!createAlgorithm ("EL::AnaAlgorithm", "DummyAlg." + m_name, algorithm).isSuccess())
      {
        ANA_MSG_ERROR ("failed to create dummy algorithm to hold public tool " + m_name);
        return StatusCode::FAILURE;
      }
    }

    for (auto& cleanup : componentMap.m_cleanup)
      algorithm->addCleanup (cleanup);

    ANA_MSG_DEBUG ("Created component of type " << m_type);
    return StatusCode::SUCCESS;
  }
}
