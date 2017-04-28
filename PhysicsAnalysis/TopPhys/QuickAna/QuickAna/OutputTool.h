/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__OUTPUT_TOOL_H
#define QUICK_ANA__OUTPUT_TOOL_H

#include <QuickAna/Global.h>

#include <AsgTools/AsgTool.h>
#include <QuickAna/IOutputTool.h>
#include <QuickAna/xAODInclude.h>
#include <QuickAna/xAODEventSelect.h>
#include <xAODCore/ShallowAuxContainer.h>
#include <xAODCore/ShallowAuxInfo.h>
#include <xAODMissingET/MissingETAuxContainer.h>

namespace ana
{

  /// Simple tool for copying an object from the input persistent
  /// store to the output persistent store in ROOT. This actually
  /// doesn't require any type or aux details because TEvent can
  /// do it all with just the string key.
  ///
  /// The copying of objects to the output will be necessary
  /// for the current mode of operation in QuickAna, where shallow
  /// copies are made from the input containers.
  ///
  /// Note: this won't compile in Athena due to the dependence on
  /// xAODRootAccess via TEvent.
  ///
  /// THIS TOOL IS DEPRECATED.
  /// Please use the OutputToolXAOD and MasterOutputToolXAOD instead.
  ///
  class OutputCopyTool : public IOutputToolOld, public asg::AsgTool
  {

    ASG_TOOL_CLASS(OutputCopyTool, IOutputToolOld)

  public:

    /// Standard constructor
    OutputCopyTool(const std::string& name, const std::string& contName="");

    /// Initialize the tool
    virtual StatusCode initialize() override;

    /// Copy the container(s) from the input store to the output store
    /// with eventstore key "basename". Suffix is not applied.
    virtual StatusCode write(xAOD::TEvent* outputStore,
                             const std::string& suffixNotUsed = "") override;

  protected:

    /// Name of the container to write out
    std::string m_contName;

  }; // class OutputCopyTool



  /// Templated tool class for writing a container and its aux container
  /// to the persistent event store in ROOT. The first template parameter
  /// specifies the xAOD container type. The second template parameter
  /// specifies the auxilliary store type. The default aux type is
  /// ShallowAuxContainer.
  ///
  /// For now, I'll just inherit from AsgTool. I could change this later.
  ///
  /// Note: this won't compile in Athena due to the dependence on
  /// xAODRootAccess via TEvent.
  ///
  /// THIS TOOL IS DEPRECATED.
  /// Please use the OutputToolXAOD and MasterOutputToolXAOD instead.
  ///
  template<class ContainerType, class AuxType = xAOD::ShallowAuxContainer>
  class OutputTool : public IOutputToolOld,
                     public asg::AsgTool
  {

    // Is this allowed?
    ASG_TOOL_CLASS(OutputTool, IOutputToolOld)

  public:

    /// Standard constructor
    OutputTool(const std::string& name, const std::string& contBaseName="");

    /// Initialize the tool
    virtual StatusCode initialize() override;

    /// Write the container and its aux container to the provided
    /// persistent store. The container name is "basename_suffix".
    /// Returns FAILURE if the templated types do not match those in the store.
    virtual StatusCode write(xAOD::TEvent* outputStore,
                             const std::string& suffix = "") override;

  protected:

    /// Name of the container to write out
    std::string m_contBaseName;

  }; // class OutputTool



  /// Convenience typedefs
  /// @{
  typedef OutputTool<xAOD::EventInfo, xAOD::ShallowAuxInfo> EventInfoOutputTool;
  typedef OutputTool<EventSelectType, xAOD::ShallowAuxInfo> EventSelectOutputTool;
  typedef OutputTool<xAOD::ElectronContainer> ElectronOutputTool;
  typedef OutputTool<xAOD::MuonContainer> MuonOutputTool;
  typedef OutputTool<xAOD::TauJetContainer> TauOutputTool;
  typedef OutputTool<xAOD::JetContainer> JetOutputTool;
  typedef OutputTool<xAOD::PhotonContainer> PhotonOutputTool;
  typedef OutputTool<xAOD::MissingETContainer, xAOD::MissingETAuxContainer>
          MetOutputTool;
  /// @}

} // namespace ana

#include <QuickAna/OutputTool.icc>

#endif
