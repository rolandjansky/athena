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

#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODJet/JetAuxContainer.h"

#include "JetInterface/IJetProvider.h"
#include "AsgTools/AsgTool.h"

#include "JetRec/PseudoJetContainer.h"


class JetTrimming
: public asg::AsgTool,
  virtual public JetProvider<xAOD::JetAuxContainer> {
  ASG_TOOL_CLASS(JetTrimming, IJetProvider)

public:

  using asg::AsgTool::AsgTool;

  StatusCode initialize() override;

  /// Return the final jets with their aux store.
  /// Can return a pair of null pointers if an error occurs.
  std::pair<std::unique_ptr<xAOD::JetContainer>, std::unique_ptr<SG::IAuxStore> > getJets() const override;


protected:
    

  /// Handle Input JetContainer (this contains the parent ungroomed jets to be trimmed) 
  SG::ReadHandleKey<xAOD::JetContainer> m_inputJetContainer {this, "InputJetContainer", "ungroomedinput", "Input ungroomed jet container"};

  /// This is the input to the parent JetContainer. It is needed in order to re-assign the ghost constituents from the final trimmed PJ to the xAOD::Jet
  SG::ReadHandleKey<PseudoJetContainer> m_inputPseudoJets {this, "InputPseudoJets", "inputpseudojet", "input constituents of parent JetContainer"};
  
  
  // Job options.
  Gaudi::Property<float> m_rclus        {this, "RClus", 0.3 , "R for reclustering (0 for none)"}; 
  Gaudi::Property<float> m_ptfrac       {this, "PtFrac", 0.03, "pT fraction for retaining subjets"};
  Gaudi::Property<std::string> m_finalPseudoJets       {this, "TrimmedOutputPseudoJets", "undef", "Key to save the final trimmed pj. Necessary in order to ensure a valid PJ can be linked from xAOD::Jet"};
  
  
};

#endif
