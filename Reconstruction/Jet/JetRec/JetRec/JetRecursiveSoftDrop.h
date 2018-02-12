// JetRecursiveSoftDrop.h

#ifndef JetRecursiveSoftDrop_H
#define JetRecursiveSoftDrop_H

// Jennifer Roloff
// August 2017
//
// Tool to groom jets with recursive softdrop.
// SoftDrop is described in this paper: arXiv:1402.2657 (recursive softdrop may not have its own documentation yet)
// See also: https://fastjet.hepforge.org/trac/browser/contrib/contribs/RecursiveTools/tags/2.0.0-beta1/RecursiveSoftDrop.hh

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetGroomer.h"
#include "JetInterface/IJetFromPseudojet.h"
#include "AsgTools/ToolHandle.h"

#include "fastjet/ClusterSequence.hh"
#include "fastjet/contrib/RecursiveSymmetryCutBase.hh"
#include "fastjet/contrib/RecursiveSoftDrop.hh"

class JetRecursiveSoftDrop
: public asg::AsgTool,
  virtual public IJetGroomer {
ASG_TOOL_CLASS(JetRecursiveSoftDrop, IJetGroomer)

public:

  // Ctor.
  JetRecursiveSoftDrop(std::string name);

  // Dtor.
  ~JetRecursiveSoftDrop();

  // Initilization.
  StatusCode initialize();

  // Groom a jet and add result to a container.
  int groom(const xAOD::Jet& jin, xAOD::JetContainer& jout) const;

  // Dump to log.
  void print() const;

private:  // data

  // Job options.
  // Recursive SoftDrop algorithm:
  // z > zcut * (dR12/R0)^beta
  // z = min(pT1, pT2)/(pT1+pT2)
  // N : algorithm terminates after passing SoftDrop condition N times 
  // (Standard SoftDrop terminates after passing it once)
  float m_zcut;                        // pT fraction for retaining subjets
  float m_beta;                        // How much to consider angular dependence
  int m_N;                        // Number of layers (-1 <> infinite)
  ToolHandle<IJetFromPseudojet> m_bld;  // Tool to build jets.

};

#endif
