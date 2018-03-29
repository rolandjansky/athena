/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetPseudojetCopier.h

#ifndef JETREC_JetPseudojetCopier_H
#define JETREC_JetPseudojetCopier_H

//////////////////////////////////////////////////////
/// \class JetPseudojetCopier
/// \author Pierre-Antoine Delsart and David Adams
/// \date November 2014
///
/// Copy the pseudojets from one jet container to another.
/// This enables the rebuilding of a container read from file so it can
/// be used as input to grooming or other tools that mase use of the fastjet
/// cluster sequence.
///
/// Properties:
///   DestinationContainer: Name of the jet container to which the current
///     pseudojets are associated.
///   Label: Label for this pseudojet mapping (SG name for the map).
//////////////////////////////////////////////////////

#include <string>
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "JetInterface/IJetConsumer.h"
#include "JetInterface/IJetPseudojetRetriever.h"

class JetPseudojetCopier
: virtual public asg::AsgTool,
  virtual public IJetConsumer {

  ASG_TOOL_CLASS(JetPseudojetCopier, IJetConsumer)

public:
    
  /// Ctor from tool name.
  JetPseudojetCopier(const std::string& myname);
 
  /// Intialization.
  StatusCode initialize();

  /// Copy pseudojets from srcjets to dstjets.
  int copy(const xAOD::JetContainer& srcjets,
           const xAOD::JetContainer& dstjets,
           std::string label ="PseudojetMap") const;

  /// Copy the pseudojets from srcjets to container with name dstname.
  int copy(const xAOD::JetContainer& srcjets,
           std::string dstname, std::string label ="PseudojetMap") const;

  // Process the input jets using the
  int process(const xAOD::JetContainer& srcjets) const;

private:

  /// Properties.
  std::string m_dstname;
  std::string m_label;
  ToolHandle<IJetPseudojetRetriever> m_hpjr;

};

#endif
