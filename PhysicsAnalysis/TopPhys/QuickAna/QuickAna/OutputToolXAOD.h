/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__OUTPUT_TOOL_XAOD_H
#define QUICK_ANA__OUTPUT_TOOL_XAOD_H

// Shouldn't be used in Athena
#ifdef ROOTCORE

#include "AsgTools/AsgTool.h"

#include "QuickAna/Global.h"
#include "QuickAna/IOutputTool.h"

namespace ana
{

  /// @class OutputToolXAOD
  /// @brief A tool which can write collections of a particular object type
  /// to the persistent output store.
  ///
  /// This tool can handle both a list of container names to copy from input
  /// file to output file (like the original container of a shallow copy) as
  /// well as a list of containers and their aux containers to take from the
  /// transient store and write to the output persistent store.
  ///
  /// For now, this tool is not really dual-use. It uses the RootCore-specific
  /// implementation of the event stores and thus won't compile in Athena.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  template < class ContainerType, class AuxContainerType >
  class OutputToolXAOD : public IOutputTool, public asg::AsgTool
  {

      /// We don't actually need an Athena constructor currently
      ASG_TOOL_CLASS(OutputToolXAOD, IOutputTool)

    public:

      /// Standard constructor
      OutputToolXAOD(const std::string& name);

      /// Initialize the tool
      virtual StatusCode initialize() override;

      /// Execute the output writing
      virtual StatusCode write() override;

    private:

      /// List of containers to retrieve from transient store and write
      /// to the persistent output store.
      std::vector<std::string> m_writeContNames;

      /// List of container names to copy directly from input to output.
      std::vector<std::string> m_copyContNames;

  }; // class OutputToolXAOD

} // namespace ana

#endif // ROOTCORE

#include "QuickAna/OutputToolXAOD.icc"

#endif
