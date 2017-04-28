/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__OBJECTS_OUTPUT_TOOL_H
#define QUICK_ANA__OBJECTS_OUTPUT_TOOL_H

#include <QuickAna/Global.h>

#include <AsgTools/AsgTool.h>
#include <QuickAna/IOutputTool.h>
#include <AsgTools/AnaToolHandle.h>

namespace ana
{

  /// Tool class for writing out all analysis objects
  ///
  /// THIS TOOL IS DEPRECATED.
  /// Please use the OutputToolXAOD and MasterOutputToolXAOD instead.
  ///
  class ObjectsOutputTool : public IOutputToolOld,
                            public asg::AsgTool
  {

    /// Declare the tool to the framework
    ASG_TOOL_CLASS(ObjectsOutputTool, IOutputToolOld)

  public:

    /// Standard constructor
    ObjectsOutputTool(const std::string& name);

    /// Initialize the tool
    virtual StatusCode initialize() override;

    /// Write all containers and their output aux containers to the provided
    /// persistent store. This method loops over the configured objects and
    /// configured suffixes, writing each out as basename_suffix.
    /// The user must still call TEvent::fill in order to save an event
    /// to the output file.
    __attribute__ ((deprecated))
    virtual StatusCode execute(xAOD::TEvent* outputStore);

    /// Write each object type and associated aux containers to the provided
    /// persistent store with the provided suffix as basename_suffix. This
    /// method form allows the client to insert this tool into an existing
    /// loop over suffixes (i.e., systematics).
    virtual StatusCode write(xAOD::TEvent* outputStore,
                             const std::string& suffix = "") override;

    /// Add an output which will be copied from the input store
    StatusCode addOutputCopyObject(const std::string& name);

    /// Add an output container by type and basename.
    StatusCode addOutputObject(ana::ObjectType objType,
                               const std::string& basename);

    /// Add an already configured output tool. This method allows users to
    /// configure the individual output container tools as desired.
    /// User must fully initialize the IOutputToolOld before this.
    /// This tool will take ownership of the IOutputToolOld.
    //StatusCode addOutputTool(std::unique_ptr<IOutputToolOld> tool);


    /// Set object suffixes via a list of systematics
    void setSuffixes(const std::vector<CP::SystematicSet>& sysList);

  private:

    /// List of suffixes (e.g. systematics names) to process
    std::vector<std::string> m_suffixes;

    /// Object output tools for each configured type
    std::vector< asg::AnaToolHandle<IOutputToolOld> > m_outputTools;

    /// Object output tools for each configured type
    //std::vector< std::unique_ptr<IOutputToolOld> > m_outputTools;

  }; // class ObjectsOutputTool

} // namespace ana

#endif
