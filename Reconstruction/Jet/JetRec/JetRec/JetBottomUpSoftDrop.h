/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetBottomUpSoftDrop.h

#ifndef JetBottomUpSoftDrop_H
#define JetBottomUpSoftDrop_H

// Jennifer Roloff & Joe Taenzer
// October 2016
//
// Tool to groom jets with bottom up softdrop.
// SoftDrop is described in this paper: arXiv:1402.2657 
// Bottom Up SoftDrop is described in this paper (under the name "Iterated SoftDrop": https://arxiv.org/pdf/1704.06266.pdf
// See also: https://fastjet.hepforge.org/trac/browser/contrib/contribs/RecursiveTools/tags/2.0.0-beta1/BottomUpSoftDrop.hh

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetGroomer.h"
#include "JetInterface/IJetFromPseudojet.h"
#include "AsgTools/ToolHandle.h"

#include "fastjet/ClusterSequence.hh"
#include "fastjet/contrib/RecursiveSymmetryCutBase.hh"
#include "fastjet/contrib/BottomUpSoftDrop.hh"

class JetBottomUpSoftDrop
: public asg::AsgTool,
  virtual public IJetGroomer {
ASG_TOOL_CLASS(JetBottomUpSoftDrop, IJetGroomer)

public:

  // Ctor.
  JetBottomUpSoftDrop(std::string name);

  // Dtor.
  ~JetBottomUpSoftDrop();

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
  // SoftDrop algorithm:
  // z > zcut * (dR12/R0)^beta
  // z = min(pT1, pT2)/(pT1+pT2)
  // R0 = characteristic jet radius
  float m_zcut;                        // pT fraction for retaining subjets
  float m_beta;                        // How much to consider angular dependence
  float m_R0;	                         // Normalization of angular distance, usually the characteristic jet radius (default R0 = 1)
  ToolHandle<IJetFromPseudojet> m_bld;  // Tool to build jets.

};

#endif
