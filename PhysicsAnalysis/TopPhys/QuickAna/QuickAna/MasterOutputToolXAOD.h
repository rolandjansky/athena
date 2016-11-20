/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__MASTER_OUTPUT_TOOL_XAOD_H
#define QUICK_ANA__MASTER_OUTPUT_TOOL_XAOD_H

// Shouldn't be used in Athena
#ifdef ROOTCORE

// Infrastructure includes
#include "AsgTools/AsgTool.h"

// Local includes
#include "QuickAna/Global.h"
#include "QuickAna/IOutputTool.h"
#include "QuickAna/EventData.h"
#include "AsgTools/AnaToolHandle.h"

namespace ana
{

  /// @class MasterOutputToolXAOD
  /// @brief A tool which manages and applies a set of IOutputTools.
  ///
  /// This tool can be configured from the EventData object.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class MasterOutputToolXAOD : public IOutputTool, public asg::AsgTool
  {

      /// We don't actually need an Athena constructor currently
      ASG_TOOL_CLASS(MasterOutputToolXAOD, IOutputTool)

    public:

      /// Standard constructor
      MasterOutputToolXAOD(const std::string& name);

      /// Initialize the tool
      virtual StatusCode initialize() override;

      /// Execute the output writing
      virtual StatusCode write() override;

    private:

      /// EventData meta-data for holding container information.
      EventData m_eventData;

      /// List of object types to write out. Defaults to 'all'.
      std::vector<ObjectType> m_outputTypes;

      /// Handle array of output tools for each object type.
      std::vector< asg::AnaToolHandle<IOutputTool> > m_outputTools;

  }; // class MasterOutputToolXAOD

} // namespace ana

#endif // ROOTCORE

#endif
