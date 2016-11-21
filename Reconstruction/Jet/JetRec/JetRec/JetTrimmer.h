/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetTrimmer.h

#ifndef JetTrimmer_H
#define JetTrimmer_H

// David Adams
// January 2014
//
// Tool to groom jets by trimming.

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetGroomer.h"
#include "JetInterface/IJetFromPseudojet.h"
#include "AsgTools/ToolHandle.h"

class JetTrimmer
: public asg::AsgTool,
  virtual public IJetGroomer {
ASG_TOOL_CLASS(JetTrimmer, IJetGroomer)

public:

  // Ctor.
  JetTrimmer(std::string name);

  // Dtor.
  ~JetTrimmer();

  // Initilization.
  StatusCode initialize();

  // Groom a jet and add result to a container.
  int groom(const xAOD::Jet& jin, xAOD::JetContainer& jout) const;

  // Dump to log.
  void print() const;

private:  // data

  // Job options.
  float m_rclus;                        // R for reclustering (0 for none)
  float m_ptfrac;                       // pT fraction for retaining subjets
  ToolHandle<IJetFromPseudojet> m_bld;  // Tool to build jets.

};

#endif
