/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TriggerPseudoJetGetter2.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigHLTJetRec
//
//
// Description: TriggerPseudoJetGetter2 is a dual-use tool to retrieve and 
// build the pseudojet inputs used in jet reconstruction.
// 
// ********************************************************************

#ifndef TRIGHLTJETREC_TRIGGERPSEUDOJETGETTER2_H
#define TRIGHLTJETREC_TRIGGERPSEUDOJETGETTER2_H

#include "TrigHLTJetRec/ITriggerPseudoJetGetter2.h"
#include "JetRec/PseudoJetContainer.h"
// #include "JetRec/PseudoJetGetter.h"
// #include "JetEDM/PseudoJetVector.h"
#include "xAODJet/JetContainerInfo.h"
#include "AsgTools/AsgTool.h"
#include <string>

class TriggerPseudoJetGetter2: virtual public ITriggerPseudoJetGetter2,
  public asg::AsgTool {
  ASG_TOOL_CLASS(TriggerPseudoJetGetter2, ITriggerPseudoJetGetter2) 

 public:
	  
  TriggerPseudoJetGetter2(const std::string& name);

  ~TriggerPseudoJetGetter2();

  StatusCode initialize() override;

  // Stores a vector of pseudo jets to be used in get()
  virtual void prime(const xAOD::CaloClusterContainer*) override;

  // Returns the pseudojet collection
  const PseudoJetVector* get() const override;

  // Write properties to the log
  void print() const override;

  /// Label assignd to this collection of pseudojets.
  virtual std::string label() const override;

  // methods not relevant for the trigger - use will give a runtime error.
  // virtual StatusCode createAndRecord() const override;

  virtual int 
    outputContainerNames(std::vector<std::string>& connames) override;
  virtual int 
    inputContainerNames(std::vector<std::string>& connames) override;

 private: 

 std::string m_label; // will be converted to a xAOD::JetInput::Type
 std::unique_ptr<const PseudoJetContainer> m_pseudoJetContainer;

 // concrete type used for pseudojet selection
 xAOD::JetInput::Type m_inputType;
 bool m_noNegE{false};   // used for pseudojet selection.
};

#endif
