/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_TOOLS_ASG_TOOL_CONFIG_H
#define ASG_TOOLS_ASG_TOOL_CONFIG_H

#ifndef ROOTCORE
#error only include this header in AnalysisBase
#endif

#include <AsgTools/AsgComponentConfig.h>

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
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   configuration errors\n
    ///   algorithm creation/initialization errors
  public:
    template<typename T> ::StatusCode
    makeTool (std::unique_ptr<T>& tool) const;



    //
    // private interface
    //
  };



  //
  // template methods
  //

  template<typename T> ::StatusCode AsgToolConfig ::
  makeTool (std::unique_ptr<T>& tool) const
  {
    using namespace msgComponentConfig;

    ANA_CHECK (makeComponentExpert (tool, "new %1% (\"%2%\")", true));
    ANA_CHECK (tool->initialize());

    ANA_MSG_DEBUG ("Created component of type " << type());
    return StatusCode::SUCCESS;
  }
}

#endif
