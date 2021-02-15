/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// JetReclusterer.h

#ifndef JetReclusterer_H
#define JetReclusterer_H

/// \class JetReclusterer
/// \author David Adams
/// \date November 2014
///
/// Tool to recluster jets, i.e. refind starting from the parent jets constituents.
///
/// Properties:
/// * JetFinder - Tool that does the jet finding.
/// * JetConstituentsRetriever - Tool that retrieves constituents from input jet.

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetGroomer.h"
#include "JetInterface/IJetConstituentsRetriever.h"
#include "JetInterface/IJetFinder.h"
#include "AsgTools/ToolHandle.h"

class JetReclusterer
: public asg::AsgTool,
  virtual public IJetGroomer {
ASG_TOOL_CLASS(JetReclusterer, IJetGroomer)

public:

  // Ctor.
  JetReclusterer(std::string name);

  // Dtor.
  ~JetReclusterer();

  // Initilization.
  virtual StatusCode initialize() override;

  // Groom a jet and add result to a container.
  virtual int groom(const xAOD::Jet& jin,
                    const PseudoJetContainer&,
                    xAOD::JetContainer& jout) const override;

  // Dump to log.
  virtual void print() const override;

private:  // data

  // Job options.
  ToolHandle<IJetConstituentsRetriever> m_hcr;  // Tool to retrieve jet constituents.
  ToolHandle<IJetFinder> m_finder;              // Tool to build jets.

};

#endif
