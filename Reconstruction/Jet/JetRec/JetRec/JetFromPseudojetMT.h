// JetFromPseudojetMT.h -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef JetFromPseudojetMT_H
#define JetFromPseudojetMT_H


// Tool to construct a jet from a pseudojet. The pseudojet,
// kinematics, constituents and selected attributes are added to the jet.

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetFromPseudojetMT.h"
#include "fastjet/PseudoJet.hh"
#include "xAODJet/JetContainer.h"

class JetFromPseudojetMT
: public asg::AsgTool,
  virtual public IJetFromPseudojetMT {
ASG_TOOL_CLASS(JetFromPseudojetMT, IJetFromPseudojetMT)

public:

  // Ctor.
  JetFromPseudojetMT(std::string name);

  // Initialization.
  StatusCode initialize();

  // Construct jet from pseudojet and input type and add to a container.
  // Return a pointer to the new jet or null for failure.
  // If pparent is not null, then it is set as the parent and its
  // configuration properties are copied.
  virtual xAOD::Jet* add(const fastjet::PseudoJet&,
                         const PseudoJetContainer&,
                         xAOD::JetContainer&,
                         xAOD::JetInput::Type inputtype) const override;


  // Construct jet from pseudojet and parent and add to a container.
  // Return a pointer to the new jet or null for failure.
  // If pparent is not null, then it is set as the parent and its
  // configuration properties are copied.
  virtual xAOD::Jet* add(const fastjet::PseudoJet& pj,
                         const PseudoJetContainer&,
                         xAOD::JetContainer& jets,
                         const xAOD::Jet* pparent) const override;

  // Dump to log.
  void print() const;


protected:

  // Method use by the above add(...) to do most of the work.
  xAOD::Jet* addjet(const fastjet::PseudoJet& pj,
                    const PseudoJetContainer&,
                    xAOD::JetContainer& jets,
                    const xAOD::Jet* pparent) const;

  // Method to evalaute the EM scale momentum
  void buildAndSetEMScaleMom(xAOD::Jet * jet,
                             xAOD::JetInput::Type inputtype) const ;

private:  //data

  // Job options.
  std::vector<std::string> m_atts;    // List of jet attributes.

  bool m_doArea;
  bool m_doFourVecArea;

  // bool m_isTrigger;
};

#endif
