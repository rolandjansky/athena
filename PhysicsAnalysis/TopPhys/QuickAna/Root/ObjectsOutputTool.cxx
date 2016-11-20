/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef ROOTCORE

// Local includes
#include <QuickAna/ObjectsOutputTool.h>

#include <PATInterfaces/SystematicSet.h>
#include <QuickAna/OutputTool.h>

namespace
{

  using ToolHandleList_t = std::vector< asg::AnaToolHandle<ana::IOutputToolOld> >;

  /// Local template helper function to construct and initialize one output tool
  template < class OutputToolType >
  StatusCode constructTool(ToolHandleList_t& outputTools,
                           asg::AsgTool* parent,
                           const std::string& toolName,
                           const std::string& contPropName,
                           const std::string& contName)
  {
    using namespace ana::msgObjectDefinition;

    outputTools.emplace_back(toolName, parent);
    auto& handle = outputTools.back();
    ANA_CHECK( ASG_MAKE_ANA_TOOL(handle, OutputToolType) );
    ANA_CHECK( handle.setProperty(contPropName, contName) );
    ANA_CHECK( handle.initialize() );
    return StatusCode::SUCCESS;
  }

}

namespace ana
{

  //---------------------------------------------------------------------------
  // Tool constructor
  //---------------------------------------------------------------------------
  ObjectsOutputTool::ObjectsOutputTool(const std::string& name)
    : asg::AsgTool(name)
  {
    declareProperty("Suffixes", m_suffixes);
  }


  //---------------------------------------------------------------------------
  // Initialize the tool
  //---------------------------------------------------------------------------
  StatusCode ObjectsOutputTool::initialize()
  {
    ATH_MSG_WARNING("This tool is deprecated. Please move to the new " <<
                    "OutputToolXAOD and MasterOutputToolXAOD.");
    return StatusCode::SUCCESS;
  }


  //---------------------------------------------------------------------------
  // Execute the output writing for all systematics
  //---------------------------------------------------------------------------
  StatusCode ObjectsOutputTool::execute(xAOD::TEvent* outputStore)
  {
    // Loop over suffixes
    for(const std::string& suffix : m_suffixes){
      ATH_CHECK( write(outputStore, suffix) );
    }
    return StatusCode::SUCCESS;
  }


  //---------------------------------------------------------------------------
  // Invoke the output writing tools for one systematic suffix
  //---------------------------------------------------------------------------
  StatusCode ObjectsOutputTool::write(xAOD::TEvent* outputStore,
                                      const std::string& suffix)
  {
    // Loop over all tools
    for(auto& tool : m_outputTools){
      ATH_CHECK( tool->write(outputStore, suffix) );
    }
    return StatusCode::SUCCESS;
  }


  //---------------------------------------------------------------------------
  // Add an OutputCopyTool to the configuration
  //---------------------------------------------------------------------------
  StatusCode ObjectsOutputTool::addOutputCopyObject(const std::string& contName)
  {
    std::string toolName = name() + "_input_" + contName;
    ATH_CHECK (
      constructTool<OutputCopyTool>(m_outputTools, this, toolName,
                                    "ContainerName", contName)
    );
    return StatusCode::SUCCESS;
  }


  //---------------------------------------------------------------------------
  // Add an OutputTool to the configuration
  //---------------------------------------------------------------------------
  StatusCode ObjectsOutputTool::addOutputObject(ana::ObjectType objType,
                                                const std::string& basename)
  {
    ATH_MSG_INFO("addOutputObject " << basename);
    if(objType == OBJECT_EVENTINFO) {
      ATH_CHECK (
        constructTool<EventInfoOutputTool>
          (m_outputTools, this, name()+"_eventInfo", "ContainerBaseName", basename)
      );
    }

    else if(objType == OBJECT_EVENT_SELECT) {
      ATH_CHECK (
        constructTool<EventSelectOutputTool>
          (m_outputTools, this, name()+"_eventSelect", "ContainerBaseName", basename)
      );
    }

    else if(objType == OBJECT_ELECTRON) {
      ATH_CHECK (
        constructTool<ElectronOutputTool>
          (m_outputTools, this, name()+"_electrons", "ContainerBaseName", basename)
      );
    }

    else if(objType == OBJECT_MUON) {
      ATH_CHECK (
        constructTool<MuonOutputTool>
          (m_outputTools, this, name()+"_muons", "ContainerBaseName", basename)
      );
    }

    else if(objType == OBJECT_TAU) {
      ATH_CHECK (
        constructTool<TauOutputTool>
          (m_outputTools, this, name()+"_taus", "ContainerBaseName", basename)
      );
    }

    else if(objType == OBJECT_JET) {
      ATH_CHECK (
        constructTool<JetOutputTool>
          (m_outputTools, this, name()+"_jets", "ContainerBaseName", basename)
      );
    }

    else if(objType == OBJECT_PHOTON) {
      ATH_CHECK (
        constructTool<PhotonOutputTool>
          (m_outputTools, this, name()+"_photons", "ContainerBaseName", basename)
      );
    }

    else if(objType == OBJECT_MET) {
      ATH_CHECK (
        constructTool<MetOutputTool>
          (m_outputTools, this, name()+"_met", "ContainerBaseName", basename)
      );
    }

    else if(objType == OBJECT_MET2) {
      ATH_CHECK (
        constructTool<MetOutputTool>
          (m_outputTools, this, name()+"_met2", "ContainerBaseName", basename)
      );
    }

    else {
      ATH_MSG_ERROR("Unsupported object type: " << objType);
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }


  //---------------------------------------------------------------------------
  // Add an existing OutputTool to the configuration.
  // I'd prefer to use handle properties than this method.
  //---------------------------------------------------------------------------
  //StatusCode ObjectsOutputTool::addOutputTool(std::unique_ptr<IOutputToolOld> tool)
  //{
  //  m_outputTools.push_back(std::move(tool));
  //  return StatusCode::SUCCESS;
  //}


  //---------------------------------------------------------------------------
  // Configure the systematic suffixes
  //---------------------------------------------------------------------------
  void ObjectsOutputTool::setSuffixes
  (const std::vector<CP::SystematicSet>& sysList)
  {
    for(const auto& syst : sysList)
      m_suffixes.push_back(syst.name());
  }

} // namespace ana

#endif
