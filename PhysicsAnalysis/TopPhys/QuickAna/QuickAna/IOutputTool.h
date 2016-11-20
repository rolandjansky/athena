/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__I_OUTPUT_TOOL_H
#define QUICK_ANA__I_OUTPUT_TOOL_H

#include <QuickAna/Global.h>

#include <AsgTools/IAsgTool.h>

namespace ana
{

  /// @class IOutputTool
  /// @brief Interface for tools that write persistent objects.
  ///
  /// The persistent format could be an xAOD file, a flat ntuple, etc.
  ///
  /// @author Steve Farrell <Steven.Farrell.cern.ch>
  ///
  class IOutputTool : virtual public asg::IAsgTool
  {

      /// Interface declaration
      ASG_TOOL_INTERFACE(IOutputTool)

    public:

      /// Virtual destructor
      virtual ~IOutputTool(){};

      /// Write the configured containers to the output.
      virtual StatusCode write() = 0;

  }; // class IOutputTool


  /// Common tool interface class for tools that write containers to
  /// a persistent output format. That format could be an xAOD file,
  /// a flat ntuple, etc.
  ///
  /// The write method (for now) accepts a suffix, which would be used
  /// to specify the systematic-dependent container names, typically with
  /// a basename specified via configuration.
  ///
  /// This interface is still subject to change.
  ///
  class IOutputToolOld : virtual public asg::IAsgTool
  {

    /// Interface declaration
    ASG_TOOL_INTERFACE(IOutputToolOld)

  public:

    /// Virtual destructor
    virtual ~IOutputToolOld(){};

    /// Write the configured containers to the output.
    virtual StatusCode write(xAOD::TEvent* outputStore,
                             const std::string& suffix = "") = 0;

  }; // class IOutputToolOld

} // namespace ana

#endif
