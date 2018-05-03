/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetTrimmer.h

#ifndef JetTrimmerMT_H
#define JetTrimmerMT_H

// David Adams
// January 2014
//
// Tool to groom jets by trimming.

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetGroomerMT.h"
#include "JetInterface/IJetFromPseudojetMT.h"
#include "AsgTools/ToolHandle.h"

class JetTrimmerMT
: public asg::AsgTool,
  virtual public IJetGroomerMT {
ASG_TOOL_CLASS(JetTrimmerMT, IJetGroomerMT)

public:

  // Ctor.
  JetTrimmerMT(std::string name);

  // Dtor.
  ~JetTrimmerMT();

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
  float m_rclus;                        // R for reclustering (0 for none)
  float m_ptfrac;                       // pT fraction for retaining subjets
  ToolHandle<IJetFromPseudojetMT> m_bld;  // Tool to build jets.

};

#endif
