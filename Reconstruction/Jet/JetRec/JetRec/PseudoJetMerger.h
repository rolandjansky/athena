// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef JETREC_PSEUDOJETMERGER_H
#define JETREC_PSEUDOJETMERGER_H


#include "AsgTools/AsgTool.h"
#include "JetRec/PseudoJetContainer.h"
#include "JetInterface/IPseudoJetGetter.h"
#include "fastjet/PseudoJet.hh"
#include "JetEDM/PseudoJetVector.h"


class PseudoJetMerger
:  public asg::AsgTool,
   virtual public IPseudoJetGetter {
  ASG_TOOL_CLASS(PseudoJetMerger, IPseudoJetGetter)

public:

  typedef jet::PseudoJetVector PseudoJetVector;

  /// Constructor from tool name.
  PseudoJetMerger(const std::string& myname);

  /// Initialization.
  /// Can be skipped.
  virtual StatusCode initialize() override;

  /// Method to construct the PseudoJetVector and record in StoreGate
  virtual StatusCode createAndRecord() const override;

  // Kept for backward compatibity
  virtual const PseudoJetVector* get() const override;

  /// Return the label for these pseudojets.
  virtual std::string label() const override {return m_label;};


protected:  // data

  // Job options.

  Gaudi::Property<std::vector<std::string> >  m_inputPJC =  {this, "InputPJContainer", { } , "alg type : AntiKt, Kt, CA..."};

  /// Output collection name.
  SG::WriteHandleKey<PseudoJetContainer> m_outcoll = {this, "OutputContainer", "", "Merged output container name" };      

  /// Label for the collection. Set from the 1st input collection
  Gaudi::Property<std::string> m_label = {this , "Label", "merged", "Label of the collection"};


};
#endif
