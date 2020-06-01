// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// PseudoJetMerger.h 

/// PseudoJetMerger collects a set of PseudoJets into a group that can be
/// put together into a jet collection, combining the consituent collection
/// with a set of ghost collections.
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

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

class PseudoJetMerger
  : public AthReentrantAlgorithm  {

public:

  typedef jet::PseudoJetVector PseudoJetVector;

  // No need for a specialised constructor
  using AthReentrantAlgorithm::AthReentrantAlgorithm;

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
