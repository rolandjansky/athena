//        Copyright Iowa State University 2017.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


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
    /// \brief set a property on an algorithm or sub-tool
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   could not set property\n
    ///   property not found\n
    ///   sub-tool not found

    StatusCode doSetProperty (EL::AnaAlgorithm& alg,
                              const std::map<std::string,std::shared_ptr<asg::AsgTool> >& tools,
                              const std::string& name,
                              const std::string& value)
    {
      using namespace msgAlgorithmConfig;

      const auto split = name.rfind (".");
      if (split == std::string::npos)
      {
        if (!alg.setProperty (name, value).isSuccess())
        {
          ANA_MSG_ERROR ("failed to set property \"" << name << "\" with value \"" << value << "\"");
          return StatusCode::FAILURE;
        }
      } else
      {
        const std::string toolName = name.substr (0, split);
        const std::string propertyName = name.substr (split+1);
        const auto tool = tools.find (toolName);
        if (tool == tools.end())
        {
          ANA_MSG_ERROR ("trying to set property \"" << propertyName << "\" on tool \"" << toolName << "\" which has not been configured");
          return StatusCode::FAILURE;
        }
        if (!tool->second->setProperty (propertyName, value).isSuccess())
        {
          ANA_MSG_ERROR ("failed to set property \"" << propertyName << "\" with value \"" << value << "\" on tool \"" << toolName << "\"");
          return StatusCode::FAILURE;
        }
      }
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
  makeAlgorithm (std::unique_ptr<AnaAlgorithm>& algorithm,
                 std::vector<std::shared_ptr<void> >& cleanup) const
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
 
    ANA_MSG_DEBUG ("Creating tool of type " << m_type);

    // Load the ROOT dictionary, this is needed to be able to
    // instantiate the algorithm below, i.e. the code below won't load
    // dictionaries not already loaded
    TClass* algorithmClass = TClass::GetClass (m_type.c_str());
    if (!algorithmClass)
    {
      ATH_MSG_ERROR ("Unable to load class dictionary for type " << m_type);
      return StatusCode::FAILURE;
    }

    EL::AnaAlgorithm *alg = (EL::AnaAlgorithm*)
      (gInterpreter->Calc(("dynamic_cast<EL::AnaAlgorithm*>(new " + m_type + " (\"" + m_name + "\", nullptr))").c_str()));
    if (alg == nullptr)
    {
      ANA_MSG_ERROR ("failed to create algorithm of type " << m_type);
      ANA_MSG_ERROR ("make sure you created a dictionary for your algorithm");
      return StatusCode::FAILURE;
    }
    algorithm.reset (alg);

    std::map<std::string,std::shared_ptr<asg::AsgTool> > tools;
    for (auto& toolInfo : m_privateTools)
    {
      // ideally we move the makeToolRootCore out of the detail
      // namespace, but for now I just want to make this work.
      asg::AsgTool *rawTool = nullptr;
      ANA_CHECK (asg::detail::makeToolRootCore
                 (toolInfo.second, m_name + "." + toolInfo.first, rawTool));
      std::shared_ptr<asg::AsgTool> managedTool (rawTool);
      tools.insert (std::make_pair (toolInfo.first, managedTool));
      cleanup.push_back (managedTool);
    }

    for (auto& property : m_propertyValues)
    {
      ANA_CHECK (doSetProperty (*alg, tools, property.first, property.second));
    }

    // need to create the tools inside out, i.e. sub-sub-tools before
    // sub-tools, etc.  the general assumption is that the longer the
    // name of the tool the more nested it is...
    std::vector<std::pair<std::string,std::shared_ptr<asg::AsgTool> > >
      sortedTools (tools.begin(), tools.end());
    std::sort (sortedTools.begin(), sortedTools.end(), [] (auto& a, auto& b) {
        return a.first.size() > b.first.size();});
    for (const auto& tool : sortedTools)
    {
      ANA_CHECK (tool.second->initialize());

      // using that a ToolHandle initialized with a tool name will
      // retrieve that tool, not sure if that is the best strategy
      ANA_CHECK (doSetProperty (*alg, tools, tool.first, tool.second->name()));
    }
    

    ANA_MSG_DEBUG ("Created algorithm of type " << m_type);
    return StatusCode::SUCCESS;
  }
}
