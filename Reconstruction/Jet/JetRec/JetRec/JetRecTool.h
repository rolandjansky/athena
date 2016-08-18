// JetRecTool.h. This file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef JetRecTool_H
#define JetRecTool_H

/// \class JetREcTool
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
///   InputTool: Handle for the JetRecTool called to create the input
///     collection if it is not already found in the event store.
///   JetPseudojetRetriever: Tool used to retrieve the pseudojet cluster
///     sequence associated with a jet. If undefined, one is created.
///   PseudoJetGetters: Array of handles for the tools used to
///     retrieve pseudojet collections. These collections are merged
///     and used as input for jet finding.
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
#include "JetInterface/IPseudoJetGetter.h"
#include "JetInterface/IJetFinder.h"
#include "JetInterface/IJetGroomer.h"
#include "JetInterface/IJetModifier.h"
#include "JetInterface/IJetPseudojetRetriever.h"
#include "JetInterface/IJetConsumer.h"
#include "fastjet/PseudoJet.hh"
#include "TStopwatch.h"

class JetRecTool
: public asg::AsgTool,
  virtual public IJetBuildTool,
  virtual public IJetExecuteTool {
ASG_TOOL_CLASS2(JetRecTool, IJetBuildTool, IJetExecuteTool)

public:

  JetRecTool(std::string myname);

  /// Initialization. Check all tools here.
  StatusCode initialize();

  /// Finalization. Write summary report.
  StatusCode finalize();

  /// Retrieve inputs with tools and construct new
  /// jet collection.
  const xAOD::JetContainer* build() const;

  /// Call build and put jets in event store.
  int execute() const;

  /// Display the configuration.
  void print() const;

  /// Method to return the list of input containers.
  /// The names of required input containers are appended to connames.
  /// Returns nonzero for error.
  /// Default returns 0 and adds no names.
  int inputContainerNames(std::vector<std::string>& connames);

  /// Method to return the list of output containers.
  /// The names of produced output containers are appended to connames.
  /// Returns nonzero for error.
  /// Default returns 0 and adds no names.
  int outputContainerNames(std::vector<std::string>& connames);

  
  /// For trigger usage in grooming mode only : give the input ungroomed jet container.
  void setInputJetContainer(const xAOD::JetContainer* cont) ;

private:

  /// Record the container and Aux container in the event store.
  /// TAux is the type of the Aux container.
  template <typename TAux>
  int record(const xAOD::JetContainer* pjets) const;

private:

  // Properties.
  std::string m_outcoll;
  std::string m_incoll;
  ToolHandle<IJetExecuteTool> m_intool;
  ToolHandle<IJetPseudojetRetriever> m_hpjr;
  ToolHandleArray<IPseudoJetGetter> m_pjgetters;
  ToolHandle<IJetFinder> m_finder;
  ToolHandle<IJetGroomer> m_groomer;
  ToolHandleArray<IJetModifier> m_modifiers;
  ToolHandleArray<IJetConsumer> m_consumers;
  bool m_trigger;
  int m_timer;
  bool m_shallowCopy;
  bool m_warnIfDuplicate;
  bool m_overwrite;

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
  mutable std::vector<TStopwatch> m_getclocks;
  mutable std::vector<TStopwatch> m_modclocks;
  mutable std::vector<TStopwatch> m_conclocks;
};

#endif
