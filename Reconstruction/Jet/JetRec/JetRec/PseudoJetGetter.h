// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// PseudoJetGetter.h

#ifndef PseudoJetGetter_H
#define PseudoJetGetter_H

// David Adams
// January 2014

/// PseudoJetGetter is a dual-use tool to retrieve and build the pseudojet
/// inputs used in jet finding.
///
/// Tool Properties:
///  - InputCollection: Name of the input collection.
///  - OutputCollection: Name of the output collection of pseudojets.
///  - Label: Label for the constituents. See note below.
///  - SkipNegativeEnergy: Flag indicating that inputs with negative energy
/// should be ignored.
///  - GhostScale : If nonzero, the pseudojets are labeled as ghosts and
/// their four-momenta are scaled by this factor.
///
/// Note: The label is attached to the CUI (constituent user info) associated with
/// created pseudojet and should be unique for each set of pseuodjets used as input
/// for finding a particular collection of jets.
///
/// The label for a primary pseudojet getter (the first in the vector presented
/// to JetRecTool) is used to deduce the input type and must be one of the
/// following: LCTopo, EMTopo, TopoTower, Tower, Truth, Track, PFlow.
/// Names beginning with "EM" are assumed to correspond to uncalibrated clusters.

#include "AsgTools/AsgTool.h"
#include "JetRec/PseudoJetContainer.h"
#include "JetInterface/IPseudoJetGetter.h"
#include "fastjet/PseudoJet.hh"
#include "JetEDM/PseudoJetVector.h"


class PseudoJetGetter
:  public asg::AsgTool,
  virtual public IPseudoJetGetter {
  ASG_TOOL_CLASS(PseudoJetGetter, IPseudoJetGetter)

public:

  typedef jet::PseudoJetVector PseudoJetVector;

  /// Constructor from tool name.
  PseudoJetGetter(const std::string& myname);

  /// Initialization.
  /// Can be skipped.
  virtual StatusCode initialize() override;

  /// Method to construct the PseudoJetVector and record in StoreGate
  virtual StatusCode createAndRecord() const override;

  // Kept for backward compatibity
  virtual const PseudoJetVector* get() const override;

  /// Return the label for these pseudojets.
  virtual std::string label() const override;

  /// Dump to properties to the log.
  virtual void print() const override;

  /// Method to return the list of input containers.
  /// The names of required input containers are appended to connames.
  /// Returns nonzero for error.
  /// Default returns 0 and adds no names.
  virtual int inputContainerNames(std::vector<std::string>& connames) override;

  /// Method to return the list of output containers.
  /// The names of produced output containers are appended to connames.
  /// Returns nonzero for error.
  /// Default returns 0 and adds no names.
  virtual int outputContainerNames(std::vector<std::string>& connames) override;

protected:  // data

  // Job options.

  /// Input collection name.
  SG::ReadHandleKey<xAOD::IParticleContainer> m_incoll;        

  /// Output collection name.
  SG::WriteHandleKey<PseudoJetContainer> m_outcoll;      

  /// Label for the collection.
  std::string m_label;

  /// Flag indicating to skip objects with E<0.
  bool m_skipNegativeEnergy;

  /// Ghost scale factor.
  double m_ghostscale;

  /// Flag indicating to treat objects with E<0 as ghosts  (useful for HI)
  bool m_negEnergyAsGhosts;

  bool m_emtopo;        /// True if inputs are EM-scale topo clusters.
  bool m_pflow;         /// True if inputs are PFlow

private:

  /// Returns the pseudojet collection.
  /// If it already exists, the collection in the event store is returned.
  /// If not, an new collection is created and filled by calling @c appendTo.
  /// Returns null if the collection cannot be created.
  const PseudoJetContainer* getC() const;


  std::vector<fastjet::PseudoJet> 
  createPseudoJets(const xAOD::IParticleContainer* ) const;

  std::vector<fastjet::PseudoJet> 
  IParticlesToPJs(const xAOD::IParticleContainer*) const;
  
  std::vector<fastjet::PseudoJet> 
  EMToposToPJs(const xAOD::IParticleContainer*) const;
#ifndef GENERATIONBASE
  std::vector<fastjet::PseudoJet>
  PFlowsToPJs(const xAOD::IParticleContainer*) const;
#endif
};
#endif
