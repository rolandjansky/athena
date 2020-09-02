/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// PseudoJetAlgorithm.h 

/// PseudoJetAlgorithm retrieves and builds the pseudojet inputs used in jet finding
///
/// Alg Properties:
///  - InputCollection: Name of the input collection.
///  - OutputCollection: Name of the output collection of pseudojets.
///  - Label: Label for the constituents. See note below.
///  - SkipNegativeEnergy: Flag indicating that inputs with negative energy
/// should be ignored.
///  - GhostScale : If nonzero, the pseudojets are labeled as ghosts and
/// their four-momenta are scaled by this factor.
///
/// Note: The label is attached to the CUI (constituent user info) associated with
/// created pseudojet and is used to name jet moments that point to the PJs,
/// and in rare cases (EMTopo, PFlow) to toggle special treatments.

#ifndef PseudoJetAlgorithm_H
#define PseudoJetAlgorithm_H

#include <memory>
#include "fastjet/PseudoJet.hh"
#include "JetRec/PseudoJetContainer.h"

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

class PseudoJetAlgorithm : public AthReentrantAlgorithm { 

public: 

  // No need for a specialised constructor
  using AthReentrantAlgorithm::AthReentrantAlgorithm;

  /// Athena algorithm's Hooks
  virtual StatusCode  initialize() override;
  virtual StatusCode  finalize() override {};

  // Standard execute, forwards to createAndRecord
  virtual StatusCode  execute(const EventContext& ctx) const override final;

private: 

  /// Method to construct the PseudoJetContainer and record in StoreGate
  virtual std::unique_ptr<PseudoJetContainer> createPJContainer(const xAOD::IParticleContainer& cont) const;

  /// Dump to properties to the log.
  virtual void print() const;

  std::vector<fastjet::PseudoJet> 
  createPseudoJets(const xAOD::IParticleContainer* ) const;

private:

  /// Input collection name.
  SG::ReadHandleKey<xAOD::IParticleContainer> m_incoll{"InputContainer", "", "The input IParticleContainer name"};

  /// Output collection name.
  SG::WriteHandleKey<PseudoJetContainer> m_outcoll{"OutputContainer", "", "The output PseudoJetContainer name"};

  /// Label for the collection.
  Gaudi::Property<std::string> m_label{"Label", "", "String label identifying the pseudojet type"};

  /// Flag indicating to skip objects with E<0.
  Gaudi::Property<bool> m_skipNegativeEnergy{"SkipNegativeEnergy", false, "Whether to skip negative energy inputs"};

  /// Ghost scale factor.
  Gaudi::Property<double> m_ghostscale{"GhostScale", 0.0, "Scale factor to convert PJs into ghosts that don't affect jet kinematics"};

  /// Flag indicating to treat objects with E<0 as ghosts  (useful for HI)
  Gaudi::Property<bool> m_negEnergyAsGhosts{"TreatNegativeEnergyAsGhost", false, "Whether to convert negative energy inputs into ghosts"};

  /// Internal steering flags
  /// Set in initialize()
  bool m_isGhost{false}; /// Determinines whether the PJs should be made ghosts
  bool m_emtopo;         /// True if inputs are EM-scale topo clusters.
  bool m_pflow;          /// True if inputs are PFlow

}; 

#endif
