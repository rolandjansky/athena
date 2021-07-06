/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// JetConstituentsRetriever.h

#ifndef JetConstituentsRetriever_H
#define JetConstituentsRetriever_H

/// \class JetConstituentsRetriever
/// \author David Adams
/// \date November 2014
///
/// Tool to retrieve the constituents associated with a jet.
///
/// Properties:
/// * UsePseudojet - If true and pseudojet exists, constituents are obtained from it.
/// * UseJetConstituents - If true, constituents are constructed from the jet constituents.
/// * PseudojetRetriever - The tool used to retrieve the jet pseudojet.
/// * GhostLabels - List of IParticle ghosts to include as constituents.
/// * GhostScale - Scale factor for ghost four-momenta.

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetConstituentsRetriever.h"
#include "JetInterface/IJetPseudojetRetriever.h"
#include "AsgTools/ToolHandle.h"

class JetConstituentsRetriever
: public asg::AsgTool,
  virtual public IJetConstituentsRetriever {
ASG_TOOL_CLASS(JetConstituentsRetriever, IJetConstituentsRetriever)

public:

  // Ctor.
  JetConstituentsRetriever(std::string name);

  // Initialization.
  StatusCode initialize();

  /// Append constituents to the supplied vector
  int constituents(const xAOD::Jet& jet, PseudoJetVector& cons,
                   NameList* pghostlabs =nullptr) const;

  // Dump to log.
  void print() const;

private:

  bool m_usepj;
  bool m_usejetcons;
  ToolHandle<IJetPseudojetRetriever> m_hpjr;
  NameList m_glabs;
  double m_gscale;

};

#endif
