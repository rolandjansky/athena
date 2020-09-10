// JetRecTool.h. This file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef JetRecTool_H
#define JetRecTool_H

/// \class JetRecTool
/// \author David Adams.
/// \date January 2014
///
/// This is the top-level tool for jet reconstruction.
/// It creates a new jet collection from finding, grooming or copy.
/// There is a one-to-one correpondence between jet collections and
/// JetRecTool instances.
///
/// Properties are as follows:
///   OutputContainer: Name of the output jet collection
///   InputCollection: Name of the input jet collection. Required for
///     grooming or copy. If InputTool is defined, the collection name
///     is taked from the output collection name of that tool. The
///     configured value of this property should be identical to that
///     or blank.
///   InputPseudoJets: Array of ReadHandleKeys for the pseudojet collections
///   to be retrieved and passed to JetFinder. These collections are merged
///     and used as input for jet finding.
///   InputTool: Handle for the JetRecTool called to create the input
///     collection if it is not already found in the event store.
///   JetPseudojetRetriever: Tool used to retrieve the pseudojet cluster
///     sequence associated with a jet. If undefined, one is created.
///   JetFinder: Handle for the tool used to find jets.
///   JetGroomer: Handle for the tool used to groom jets.
///   JetModifiers: Array of handles of tools used to modify the jet
///     collection, e.g. to add moments sort or filter.
///   JetConsumers: Tools consuming jets to run after building jets
///
/// This tool may be used to build jets in any one of three ways:
///   1. To find jets, the JetFinder and PseudoJetGetters must be defined.
///      InputCollection, InputTool and JetGroomer should not be defined.
///   2. To groom jets, the JetGroomer and InputCollection or InputTool
///      must be defined. The JetFinder should not be defined.
///   3. To copy jets, the InputCollection or InputTool must be defined.
/// The tool may also be used to call a sequence of pseudojet builders
/// without building any jets. In this case, none of OutputCollection,
/// InputCollection, InputTool, JetFinder or jetGroomer should be defined
/// and the array of jet modifiers should be empty.

#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetBuildTool.h"
#include "JetInterface/IJetExecuteTool.h"
#include "JetInterface/IJetFinder.h"
#include "JetInterface/IJetGroomer.h"
#include "JetInterface/IJetModifier.h"
#include "JetInterface/IJetPseudojetRetriever.h"
#include "JetInterface/IJetConsumer.h"
#include "TStopwatch.h"
#include "AsgDataHandles/ReadHandleKeyArray.h"
#include "AsgDataHandles/WriteHandleKey.h"
#include "JetEDM/PseudoJetVector.h"
#include "JetRec/PseudoJetContainer.h"
#if !defined(GENERATIONBASE) && !defined(XAOD_STANDALONE)
  #include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#endif

class JetRecTool
: public asg::AsgTool,
  virtual public IJetBuildTool,
  virtual public IJetExecuteTool {
ASG_TOOL_CLASS2(JetRecTool, IJetBuildTool, IJetExecuteTool)

public:

  JetRecTool(std::string myname);

  /// Initialization. Check all tools here.
  StatusCode initialize() override;

  /// Retrieve inputs with tools and construct new
  /// jet collection.
  virtual const xAOD::JetContainer* build() const override;

  /// Call build and put jets in event store.
  int execute() const override;

  /// Display the configuration.
  void print() const override;

  /// Method to return the list of input containers.
  /// The names of required input containers are appended to connames.
  /// Returns nonzero for error.
  /// Default returns 0 and adds no names.
  int inputContainerNames(std::vector<std::string>& connames) override;

  /// Method to return the list of output containers.
  /// The names of produced output containers are appended to connames.
  /// Returns nonzero for error.
  /// Default returns 0 and adds no names.
  int outputContainerNames(std::vector<std::string>& connames) override;

  
  /// For trigger usage in grooming mode only : give the input ungroomed jet container.
  void setInputJetContainer(const xAOD::JetContainer* cont);

private:

  std::unique_ptr<xAOD::JetContainer> fillOutputContainer() const;
  // Collect all input PseudoJetContainers into a single container
  std::unique_ptr<PseudoJetContainer> collectPseudoJets() const;
  const xAOD::JetContainer* getOldJets() const;
  std::unique_ptr<xAOD::JetContainer> makeOutputContainer() const;

  std::unique_ptr<xAOD::JetContainer> findJets() const;
  std::unique_ptr<xAOD::JetContainer> groomJets() const;
  std::unique_ptr<xAOD::JetContainer> copyJets() const;

  
  // Properties.
  SG::WriteHandleKey<xAOD::JetContainer> m_outcoll {this, "OutputContainer", ""};
  SG::ReadHandleKey<xAOD::JetContainer> m_incoll {this, "InputContainer", ""};
  // The template argument should become PseudoJetContainer
  SG::ReadHandleKeyArray<PseudoJetContainer> m_psjsin {this, "InputPseudoJets", {}};

  ToolHandle<IJetExecuteTool> m_intool;
  ToolHandle<IJetPseudojetRetriever> m_hpjr;
  ToolHandle<IJetFinder> m_finder;
  ToolHandle<IJetGroomer> m_groomer;
  ToolHandleArray<IJetModifier> m_modifiers {this, "JetModifiers", {}};
  ToolHandleArray<IJetConsumer> m_consumers {this, "JetConsumers", {}};
  bool m_trigger;
  int m_timer;

  // Cached state.
  int m_initCount;
  bool m_find;
  bool m_groom;
  bool m_copy;
  xAOD::JetInput::Type m_inputtype;
  IJetFinder::NameList m_ghostlabs;
  std::vector<std::string> m_incolls;
  std::vector<std::string> m_outcolls;
  const IJetPseudojetRetriever* m_ppjr;

  // trigger hacks
  const xAOD::JetContainer* m_trigInputJetsForGrooming; // used in trigger context only

  // Clocks.
  mutable unsigned int m_nevt;
  mutable TStopwatch m_totclock;
  mutable TStopwatch m_inpclock;
  mutable TStopwatch m_actclock;
  mutable TStopwatch m_modclock;
  mutable TStopwatch m_conclock;
  mutable TStopwatch m_pjcclock;
  mutable std::vector<TStopwatch> m_modclocks;
  mutable std::vector<TStopwatch> m_conclocks;

#if !defined (GENERATIONBASE) && !defined (XAOD_STANDALONE)
  ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};
#endif

  const double m_mevtogev = 0.001;

};

#endif
