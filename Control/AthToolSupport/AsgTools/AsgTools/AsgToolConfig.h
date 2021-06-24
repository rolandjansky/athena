/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_TOOLS_ASG_TOOL_CONFIG_H
#define ASG_TOOLS_ASG_TOOL_CONFIG_H

#include <AsgTools/AsgComponentConfig.h>
#include <AsgTools/ToolHandle.h>

namespace asg
{
  class AsgTool;

  /// \brief an object that can create a \ref AsgTool

  class AsgToolConfig : public asg::AsgComponentConfig
  {
    //
    // public interface
    //

    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
  public:
    AsgToolConfig () = default;


    /// \brief initializing constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    explicit AsgToolConfig (const std::string& val_typeAndName);


    /// \brief initializing constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    explicit AsgToolConfig (const AsgComponentConfig& val_config);


    /// \brief make a tool with the given configuration
    ///
    /// Note that the exact creational patterns are not determined yet
    /// (25 Mar 20), but presumably if you are using EventLoop you
    /// would rely on EventLoop to create your tool from this object.
    /// For use within unit tests we may use this method, but we may
    /// also change the signature.
    ///
    /// We may also use this class as a replacement for \ref
    /// AnaToolHandle in which case this would at least have to be
    /// updated to at the very least work on a ToolHandle, but so far
    /// this code isn't even dual-use.
    ///
    /// The argument `allowNestedName` should probably be omitted in
    /// most cases.  I mostly added it to allow AnaToolHandle to
    /// create new tools via an AsgToolConfig (06 Jan 21), for which a
    /// number of existing users rely on making nested names.  Usually
    /// when you want a nested name you ought to set the parent on the
    /// `ToolHandle` and then use a non-nested name for the name of
    /// the config.
    ///
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   configuration errors\n
    ///   tool creation/initialization errors
  public:
    template<typename T> ::StatusCode
    makeTool (ToolHandle<T>& toolHandle,
              std::shared_ptr<void>& cleanup,
              bool allowNestedName = false) const;


    /// \brief make a private tool with the given configuration
    ///
    /// This requires a parent pointer to be set on the ToolHandle to
    /// work, and for the parent to derive from AsgComponent in
    /// AnalysisBase.  That allows to attach the cleanup of the tool
    /// to the parent component.
    ///
    /// For practical purposes, when using an AsgToolConfig to create
    /// a private tool owned by a parent tool/algorithm inside the
    /// initialize of the tool/algorithm, this is the preferred way of
    /// doing so.
    ///
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   configuration errors\n
    ///   tool creation/initialization errors
  public:
    template<typename T> ::StatusCode
    makePrivateTool (ToolHandle<T>& toolHandle) const;



    //
    // private interface
    //
  };



  //
  // template methods
  //

#ifdef XAOD_STANDALONE

  template<typename T> ::StatusCode AsgToolConfig ::
  makeTool (ToolHandle<T>& toolHandle,
            std::shared_ptr<void>& cleanup,
            bool allowNestedName) const
  {
    using namespace msgComponentConfig;

    std::string prefix = toolHandle.parentName() + ".";

    std::unique_ptr<T> tool;
    ANA_CHECK (makeComponentExpert (tool, "new %1% (\"%2%\")", allowNestedName, prefix));
    ANA_CHECK (tool->initialize());

    std::shared_ptr<T> mycleanup (tool.release());
    toolHandle = mycleanup.get();
    cleanup = mycleanup;

    ANA_MSG_DEBUG ("Created component of type " << type());
    return StatusCode::SUCCESS;
  }

  template<typename T> ::StatusCode AsgToolConfig ::
  makePrivateTool (ToolHandle<T>& toolHandle) const
  {
    using namespace msgComponentConfig;

    INamedInterface *parentNamed = toolHandle.parent();
    if (parentNamed == nullptr)
    {
      ANA_MSG_ERROR ("need to pass private ToolHandle into makePrivateTool");
      return StatusCode::FAILURE;
    }
    AsgComponent *parentComponent = dynamic_cast<AsgComponent*>(parentNamed);
    if (parentComponent == nullptr)
    {
      ANA_MSG_ERROR ("parent \"" << parentNamed->name() << "\" for makePrivateTool() does not inherit from AsgComponent");
      return StatusCode::FAILURE;
    }

    std::shared_ptr<void> cleanup;
    if (makeTool (toolHandle, cleanup).isFailure())
      return StatusCode::FAILURE;
    parentComponent->addCleanup (cleanup);
    return StatusCode::SUCCESS;
  }

#else

  template<typename T> ::StatusCode AsgToolConfig ::
  makeTool (ToolHandle<T>& toolHandle,
            std::shared_ptr<void>& /*cleanup*/,
            bool allowNestedName) const
  {
    using namespace msgComponentConfig;

    std::string prefix = toolHandle.parentName() + ".";

    ANA_CHECK (configureComponentExpert (prefix, allowNestedName));
    toolHandle.setTypeAndName (type() + "/" + name());
    ANA_CHECK (toolHandle.retrieve());

    ANA_MSG_DEBUG ("Created component of type " << type());
    return StatusCode::SUCCESS;
  }

  template<typename T> ::StatusCode AsgToolConfig ::
  makePrivateTool (ToolHandle<T>& toolHandle) const
  {
    using namespace msgComponentConfig;

    // This is safe, because the `makeTool` function in athena does
    // not use the cleanup argument.
    std::shared_ptr<void> cleanup;
    return makeTool (toolHandle, cleanup);
  }

#endif

}

#endif
