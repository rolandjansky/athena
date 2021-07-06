/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// JetPruner.h

#ifndef JetPruner_H
#define JetPruner_H

// David Adams
// January 2014
//
// Tool to groom jets with a mass-drop filter.

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetGroomer.h"
#include "JetInterface/IJetFromPseudojet.h"
#include "fastjet/JetDefinition.hh"
#include "AsgTools/ToolHandle.h"

class JetPruner
: public asg::AsgTool,
  virtual public IJetGroomer {
ASG_TOOL_CLASS(JetPruner, IJetGroomer)

public:

  // Ctor.
  JetPruner(std::string name);

  // Dtor.
  ~JetPruner();

  // Initilization.
  StatusCode initialize();

  // Groom a jet and add result to a container.
  int groom(const xAOD::Jet& jin,
            const PseudoJetContainer&,
            xAOD::JetContainer& jout) const;

  // Dump to log.
  void print() const;

private:  // data

  // Job options.
  std::string m_jetalg;                 // Kt, AntiKt, CamKt
  float m_zcut;                         // zcut: pTi, pTj > pT(i+j)
  float m_rcut;                         // rcut: Rij < rcut*2m/pT
  ToolHandle<IJetFromPseudojet> m_bld;  // Tool to build jets.

  // Data
  fastjet::JetAlgorithm m_fjalg;
  
};

#endif
