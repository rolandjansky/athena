// this file is -*- C++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETREC_JETTRIMMING_H
#define JETREC_JETTRIMMING_H
///***********************************************
///
/// \class JetTrimming
///
/// Creates a new JetContainer by running fastjet on an input PseudoJetVector
///
/// This tool implements the IJetProvider interface. The JetContainer it returns is build by
/// running the trimming procedure on each jet of the parent JetContainer the tool is acting on.
/// Obviously the parent JetContainer must be present in the evt store, but also the input PseudoJetContainer from which it has been built.

#include "fastjet/PseudoJet.hh"
#include "fastjet/tools/Filter.hh"

#include "xAODJet/JetContainer.h"

#include "JetInterface/IJetProvider.h"
#include "JetRec/JetGroomer.h"
#include "JetRec/PseudoJetContainer.h"

class JetTrimming
: virtual public JetGroomer {
  ASG_TOOL_CLASS(JetTrimming, IJetProvider)

public:

  using JetGroomer::JetGroomer;

  StatusCode initialize() override final;

  virtual void insertGroomedJet(const xAOD::Jet&, const PseudoJetContainer&, xAOD::JetContainer&, PseudoJetVector&) const override final;

private:

  // The filter object that will apply the grooming
  std::unique_ptr<fastjet::Filter> m_trimmer;
    
  // Job options.
  Gaudi::Property<float> m_rclus        {this, "RClus", 0.3 , "R for reclustering (0 for none)"}; 
  Gaudi::Property<float> m_ptfrac       {this, "PtFrac", 0.03, "pT fraction for retaining subjets"};
  
};

#endif
