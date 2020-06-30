/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// MuonSegmentPseudoJetAlgorithm.h 

/// A PseudoJetAlgorithm for muon segments (which are not IParticles)
///
/// This is needed for the punchthrough calibration correction.
/// Due to the different configuration properties and limited
/// code sharing, this does not inherit from PseudoJetAlgorithm
/// or a common base.

#ifndef MuonSegmentPseudoJetAlgorithm_H
#define MuonSegmentPseudoJetAlgorithm_H

#include <memory>
#include "fastjet/PseudoJet.hh"
#include "JetRec/PseudoJetContainer.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODMuon/MuonSegment.h"
#include "xAODMuon/MuonSegmentContainer.h"

class MuonSegmentPseudoJetAlgorithm : public AthReentrantAlgorithm { 

public: 

  // No need for a specialised constructor
  using AthReentrantAlgorithm::AthReentrantAlgorithm;

  /// Athena algorithm's Hooks
  virtual StatusCode  initialize() override final;

  // Standard execute, forwards to createAndRecord
  virtual StatusCode  execute(const EventContext& ctx) const override final;

private: 

  std::vector<fastjet::PseudoJet> createPseudoJets(const xAOD::MuonSegmentContainer& ms) const;

  /// Dump to properties to the log.
  virtual void print() const;

private:

  /// Input collection name.
  SG::ReadHandleKey<xAOD::MuonSegmentContainer> m_incoll{this, "InputContainer", "MuonSegments", "The input MuonSegmentContainer name"};

  /// Output collection name.
  SG::WriteHandleKey<PseudoJetContainer> m_outcoll{this, "OutputContainer", "PseudoJetGhostMuonSegment", "The output PseudoJetContainer name"};

  /// Label for the collection.
  Gaudi::Property<std::string> m_label{this, "Label", "GhostMuonSegment", "String label identifying the pseudojet type"};

  Gaudi::Property<float> m_pt{this, "Pt", 1.e-10, "The pT to set for the muon segments"};

}; 

#endif
