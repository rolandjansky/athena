/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TriggerPseudoJetGetter.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigHLTJetRec
//
// AUTHOR:   Valentinos Christodoulou
//
// Description: TriggerPseudoJetGetter is a dual-use tool to retrieve and 
// build the pseudojet inputs used in jet reconstruction.
// 
// ********************************************************************

#ifndef TRIGHLTJETREC_TRIGGERPSEUDOJETGETTER_H
#define TRIGHLTJETREC_TRIGGERPSEUDOJETGETTER_H

#include "TrigHLTJetRec/ITriggerPseudoJetGetter.h"
#include "JetInterface/IPseudoJetGetter.h"
#include "JetEDM/PseudoJetVector.h"
#include "AsgTools/AsgTool.h"

class TriggerPseudoJetGetter: virtual public ITriggerPseudoJetGetter,
  public asg::AsgTool {
  ASG_TOOL_CLASS(TriggerPseudoJetGetter, ITriggerPseudoJetGetter) 

 public:
	  
  TriggerPseudoJetGetter(const std::string& name);

  virtual ~TriggerPseudoJetGetter();

  virtual StatusCode initialize() override;

  // Stores a vector of pseudo jets to be used in get()
  void prime(const PseudoJetVector* ) override;

  // Returns the pseudojet collection
  const PseudoJetVector* get() const override;

  // Write properties to the log
  void print() const override;


  // method from IPseudojetCetter - some not applicable to trigger jets...
  // needed only for future changes to offline jets
  virtual StatusCode createAndRecord() const override;

  /// Label assignd to this collection of pseudojets.
  virtual std::string label() const override;
  virtual int inputContainerNames(std::vector<std::string>& connames) override;
  virtual int outputContainerNames(std::vector<std::string>& connames) override;


 private: 

  bool m_primed;         // Indicates if prime has been called before get()
  std::string m_label;   // Label for the collection.
  std::string m_outputContainer;   // junk attribute to conform to offline
  const PseudoJetVector*  m_pseudoJetVector;

};

#endif
