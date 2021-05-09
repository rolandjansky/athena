// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// PseudoJetMerger.h 

/// PseudoJetMerger collects a list of PseudoJetContainers into a single PseudoJetContainers.
/// In terms of fastjet::PseudoJet content, the output PseudoJetContainer is simply the concatenation of the inputs.
/// The ouput PseudoJetContainer also contains all the technical information necessary to tell if 
/// a given PseudoJet extracted as a constituent of a final jet is a genuine constituent or a ghost
/// and from which category it comes from.
///
/// Alg Properties:
///  - InputPJContainers: Array of input collections to merge.
///  - OutputCollection: Name of the merged output pseudojet collection.
///  - Label: Label for the constituents.

#ifndef JETREC_PSEUDOJETMERGER_H
#define JETREC_PSEUDOJETMERGER_H


#include <memory>
#include "JetRec/PseudoJetContainer.h"
#include "fastjet/PseudoJet.hh"
#include "JetEDM/PseudoJetVector.h"
#include "AsgDataHandles/ReadHandleKeyArray.h"
#include "AsgDataHandles/WriteHandleKey.h"
#include "AsgTools/PropertyWrapper.h"

#include "AnaAlgorithm/AnaReentrantAlgorithm.h"

class PseudoJetMerger
  : public EL::AnaReentrantAlgorithm  {

public:

  typedef jet::PseudoJetVector PseudoJetVector;

  // Can't use "using ctor" because of incompatiblity with pyroot in AnalysisBase
  PseudoJetMerger(const std::string & n, ISvcLocator* l) : EL::AnaReentrantAlgorithm(n,l) {}

  /// Initialization.
  /// Can be skipped.
  virtual StatusCode initialize() override;

  // Standard execute, forwards to createAndRecord
  virtual StatusCode  execute(const EventContext& ctx) const override final;

private:  // data

  // Job options.

  SG::ReadHandleKeyArray<PseudoJetContainer> m_inputPJC =  {this, "InputPJContainers", { } , "Set of input pseudojets to merge"};

  /// Output collection name.
  SG::WriteHandleKey<PseudoJetContainer> m_outcoll = {this, "OutputContainer", "", "Merged output container name" };      

  /// Label for the collection. Set from the 1st input collection
  Gaudi::Property<std::string> m_label = {this , "Label", "merged", "Label of the collection"};


};
#endif
