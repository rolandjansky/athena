/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// JetSplitter.h

#ifndef JetSplitter_H
#define JetSplitter_H

// David Adams
// January 2014
//
// Tool to groom jets with a mass-drop filter.

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetGroomer.h"
#include "JetInterface/IJetFromPseudojet.h"
#include "AsgTools/ToolHandle.h"

class JetDumper;

class JetSplitter
: public asg::AsgTool,
  virtual public IJetGroomer {
ASG_TOOL_CLASS(JetSplitter, IJetGroomer)

public:

  // Ctor.
  JetSplitter(std::string name);

  // Dtor.
  ~JetSplitter();

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
  float m_mumax;                        // Jet size parameter.
  float m_ymin;                         // pT min in MeV
  ToolHandle<IJetFromPseudojet> m_bld;  // Tool to build jets.
  float m_rclus;                        // R for reclustering (0 for none)
  bool m_bdrs;                          // If true use R = min(Rfilt/2,R)
  int m_nsubjetmax;                     // Max number of subjets to retain.

  // Internal data.
  JetDumper* m_pdmp;                    // Use to dump constituents in VERBOSE.

};

#endif
